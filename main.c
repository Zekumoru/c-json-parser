#include "app/json-parser.h"
#include <stdarg.h>
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

void printWithIndent(size_t indent, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  for (size_t i = 0; i < indent; i++)
    printf(" ");
  vprintf(fmt, args);

  va_end(args);
}

void traverse(JsonNode* node, size_t indent)
{
  if (node == NULL)
    return;

  switch (node->type)
  {
  case NULL_NODE:
    printWithIndent(indent, "NULL_NODE\n");
    break;
  case STRING_NODE:
    printWithIndent(indent, "STRING_NODE\n");
    printWithIndent(indent, "- Key: %s\n", node->key);
    printWithIndent(indent, "- Value: %s\n", node->value.v_string);
    break;
  case INTEGER_NODE:
    printWithIndent(indent, "INTEGER_NODE\n");
    printWithIndent(indent, "- Key: %s\n", node->key);
    printWithIndent(indent, "- Value: %d\n", node->value.v_int);
    break;
  case DOUBLE_NODE:
    printWithIndent(indent, "DOUBLE_NODE\n");
    break;
  case BOOLEAN_NODE:
    printWithIndent(indent, "BOOLEAN_NODE\n");
    break;
  case OBJECT_NODE:
  case ARRAY_NODE:
    JsonNode* nodeList;
    if (node->type == OBJECT_NODE)
    {
      printWithIndent(indent, "%s\n", node->key == NULL ? "ROOT_OBJECT_NODE" : "OBJECT_NODE");
      if (node->key != NULL)
        printWithIndent(indent, "- Key: %s\n", node->key);
      nodeList = node->value.v_object;
    }
    else
    {
      printWithIndent(indent, "ARRAY_NODE\n");
      nodeList = node->value.v_array;
    }

    for (size_t i = 0; i < node->vSize; i++)
      traverse(&nodeList[i], indent + 2);
    break;
  default:
    printWithIndent(indent, "UNKNOWN_NODE\n");
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
  JsonNode* root = parse(jsonFile, manager, &parserError);

  switch (parserError.type)
  {
  case INVALID_INTEGER_LITERAL:
    printError("Syntax Error", parserError.token.lineCount, parserError.token.charCount, "Invalid integer literal");
    break;
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
  traverse(root, 0);

  // DON'T FORGET TO FREE THE PARSED JSON TREE

  deleteTokenManager(manager);
  fclose(jsonFile);

  return 0;
}
