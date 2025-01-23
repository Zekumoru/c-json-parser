#include "utils.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

void* vec_alloc(void* vec, size_t* cap, const size_t size, const size_t elemSize)
{
  if (size == 0 || elemSize == 0)
    return vec;

  // Funnily enough, there's actually no condition needed to check whether to
  // resize or not since what we want is to have the minimum possible capacity
  // to hold size which can be achieved by taking the log2 of the current size,
  // flooring it then incrementing it by one.
  *cap = pow(2, floor(log2(size)) + 1);

  void* newVec = realloc(vec, *cap * elemSize);

  // If realloc returns NULL, make sure to free the old memory
  if (newVec == NULL)
    free(vec);

  return newVec;
}

void printError(const char* errorType, size_t lineCount, size_t charCount, const char* message)
{
  printf("Error: %s at line %ld, column %ld: %s\n", errorType, lineCount, charCount, message);
}

void printLexError(LexError* error)
{
  switch (error->type)
  {
  case EMPTY_FILE:
    printError("Syntax Error", error->lineCount, error->charCount, "Expected JSON content");
    break;
  case EXPECTED_END_OF_STRING:
    printError("Syntax Error", error->lineCount, error->charCount, "Expected end-of-string double quotes");
    break;
  case INVALID_BOOLEAN_LITERAL:
    printError("Syntax Error", error->lineCount, error->charCount, "Invalid Boolean literal");
    break;
  case INVALID_NULL_LITERAL:
    printError("Syntax Error", error->lineCount, error->charCount, "Invalid Boolean literal");
    break;
  case UNEXPECTED_END_OF_INPUT:
    printError("Syntax Error", error->lineCount, error->charCount, "Unexpected end-of-input token");
    break;
  case UNEXPECTED_CHARACTER:
    printError("Syntax Error", error->lineCount, error->charCount, "Unexpected character");
    break;
  }
}

void printParseError(ParserError* error)
{
  switch (error->type)
  {
  case NO_TOKEN_FOUND:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected token but none found");
    break;
  case INVALID_INTEGER_LITERAL:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Invalid integer literal");
    break;
  case INVALID_DOUBLE_LITERAL:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Invalid double literal");
    break;
  case EXPECTED_OBJECT_KEY:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected object key");
    break;
  case EXPECTED_END_OF_OBJECT_BRACE:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected end-of-object brace");
    break;
  case EXPECTED_END_OF_ARRAY_BRACE:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected end-of-array brace");
    break;
  case EXPECTED_COLON:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected colon after object key");
    break;
  case EXPECTED_COMMA:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Expected comma");
    break;
  case UNEXPECTED_TOKEN:
    printError("Syntax Error", error->token.lineCount, error->token.charCount, "Unexpected token");
    break;
  }
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

void traverse(JsonNode* node, size_t indent, bool isParentArray)
{
  if (node == NULL)
    return;

  switch (node->type)
  {
  case NULL_NODE:
    printWithIndent(indent, "- ");
    if (!isParentArray)
      printf("%s: ", node->key);
    printf("(null)\n");
    break;
  case STRING_NODE:
    printWithIndent(indent, "- ");
    if (!isParentArray)
      printf("%s: ", node->key);
    printf("%s\n", node->value.v_string);
    break;
  case INTEGER_NODE:
    printWithIndent(indent, "- ");
    if (!isParentArray)
      printf("%s: ", node->key);
    printf("%d\n", node->value.v_int);
    break;
  case DOUBLE_NODE:
    printWithIndent(indent, "- ");
    if (!isParentArray)
      printf("%s: ", node->key);
    printf("%lf\n", node->value.v_double);
    break;
  case BOOLEAN_NODE:
    printWithIndent(indent, "- ");
    if (!isParentArray)
      printf("%s: ", node->key);
    printf("%s\n", node->value.v_bool ? "true" : "false");
    break;
  case OBJECT_NODE:
  case ARRAY_NODE:
  {
    JsonNode* nodeList;
    bool hasNoKey = node->key == NULL;

    if (node->type == OBJECT_NODE)
    {
      if (!hasNoKey)
        printWithIndent(indent, "- %s:", node->key);
      nodeList = node->value.v_object;

      if (node->vSize == 0)
        printf(" {}");
      if (!hasNoKey)
        printf("\n");
    }
    else
    {
      if (!(node->isRoot || hasNoKey))
        printWithIndent(indent, "- %s:", node->key);
      nodeList = node->value.v_array;

      if (node->vSize == 0)
        printf(" []");
      if (!hasNoKey)
        printf("\n");
    }

    size_t indentAdd = hasNoKey ? 0 : 2;
    if (!node->isRoot && hasNoKey && node->type == ARRAY_NODE)
      indentAdd += 2;

    for (size_t i = 0; i < node->vSize; i++)
      traverse(&nodeList[i], indent + indentAdd, node->type == ARRAY_NODE);
    break;
  }
  default:
    printWithIndent(indent, "- [[UNKNOWN NODE]]\n");
  }
}
