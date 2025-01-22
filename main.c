#include "app/json-parser.h"
#include <stdio.h>
#include <stdlib.h>

void printError(const char* errorType, size_t lineCount, size_t charCount, const char* message)
{
  printf("Error: %s at line %ld, column %ld: %s\n", errorType, lineCount, charCount, message);
}

void printTokens(TokenManager* manager)
{
  for (size_t i = 0; i < manager->size; i++)
  {
    const Token* token = &manager->tokens[i];

    printf("Type: ");
    switch (token->type)
    {
    case NULL_LEX:
      printf("NULL_LEX");
      break;
    case STRING_LEX:
      printf("STRING_LEX");
      break;
    case INTEGER_LEX:
      printf("INTEGER_LEX");
      break;
    case DOUBLE_LEX:
      printf("DOUBLE_LEX");
      break;
    case BOOLEAN_LEX:
      printf("BOOLEAN_LEX");
      break;
    case CURLY_OPEN:
      printf("CURLY_OPEN");
      break;
    case CURLY_CLOSE:
      printf("CURLY_CLOSE");
      break;
    case BRACKET_OPEN:
      printf("BRACKET_OPEN");
      break;
    case BRACKET_CLOSE:
      printf("BRACKET_CLOSE");
      break;
    case COMMA:
      printf("COMMA");
      break;
    case COLON:
      printf("COLON");
      break;
    }
    printf("\n");
  }
}

void traverse(JsonNode* node)
{
  if (node == NULL)
    return;

  switch (node->type)
  {
  case NULL_NODE:
    printf("NodeType: NULL_NODE\n");
    break;
  case STRING_NODE:
    printf("NodeType: STRING_NODE\n");
    break;
  case INTEGER_NODE:
    printf("NodeType: INTEGER_NODE\n");
    break;
  case DOUBLE_NODE:
    printf("NodeType: DOUBLE_NODE\n");
    break;
  case BOOLEAN_NODE:
    printf("NodeType: BOOLEAN_NODE\n");
    break;
  case OBJECT_NODE:
  case ARRAY_NODE:
    JsonNode* nodeList;
    if (node->type == OBJECT_NODE)
    {
      printf("NodeType: OBJECT_NODE\n");
      nodeList = node->value.v_object;
    }
    else
    {
      printf("NodeType: ARRAY_NODE\n");
      nodeList = node->value.v_array;
    }

    for (size_t i = 0; i < node->vSize; i++)
      traverse(&nodeList[i]);
    break;
  default:
    printf("NodeType: UNKNOWN_NODE\n");
  }
}

int main()
{
  FILE* jsonFile = fopen("sample.json", "r");

  if (!jsonFile)
  {
    perror("Could not open json file.");
    exit(1);
  }

  LexError lexError;
  TokenManager* manager = lex(jsonFile, &lexError);

  switch (lexError.type)
  {
  case EXPECTED_END_OF_STRING:
    printError("Syntax Error", lexError.lineCount, lexError.charCount, "Expected end-of-string double quotes");
    break;
  case INVALID_BOOLEAN_LITERAL:
    printError("Syntax Error", lexError.lineCount, lexError.charCount, "Invalid Boolean literal");
    break;
  case INVALID_NULL_LITERAL:
    printError("Syntax Error", lexError.lineCount, lexError.charCount, "Invalid Boolean literal");
    break;
  case UNEXPECTED_END_OF_INPUT:
    printError("Syntax Error", lexError.lineCount, lexError.charCount, "Unexpected end-of-input token");
    break;
  case UNEXPECTED_CHARACTER:
    printError("Syntax Error", lexError.lineCount, lexError.charCount, "Unexpected character");
    break;
  }

  printf("LEXICAL ANALYSIS\n");
  printTokens(manager);
  printf("\n");

  ParserError parserError;
  parserError.type = NO_PARSER_ERROR;
  JsonNode* root = parse(manager, &parserError);

  switch (parserError.type)
  {
  case EXPECTED_OBJECT_KEY:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Expected object key");
    break;
  case EXPECTED_END_OF_OBJECT_BRACE:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Expected end-of-object brace");
    break;
  case EXPECTED_END_OF_ARRAY_BRACE:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Expected end-of-array brace");
    break;
  case EXPECTED_COLON:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Expected colon after object key");
    break;
  case EXPECTED_COMMA:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Expected comma");
    break;
  case UNEXPECTED_TOKEN:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Unexpected token");
    break;
  }

  printf("SYNTACTIC ANALYSIS\n");
  traverse(root);

  deleteTokenManager(manager);
  fclose(jsonFile);

  return 0;
}
