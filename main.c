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

int main()
{
  FILE* jsonFile = fopen("sample.json", "r");

  if (!jsonFile)
  {
    perror("Could not open json file.");
    exit(1);
  }

  LexError error;
  TokenManager* manager = lex(jsonFile, &error);

  switch (error.type)
  {
  case EXPECTED_END_OF_STRING:
    printError("Syntax Error", error.lineCount, error.charCount, "Expected end-of-string double quotes");
    break;
  case INVALID_BOOLEAN_LITERAL:
    printError("Syntax Error", error.lineCount, error.charCount, "Invalid Boolean literal");
    break;
  case INVALID_NULL_LITERAL:
    printError("Syntax Error", error.lineCount, error.charCount, "Invalid Boolean literal");
    break;
  case UNEXPECTED_END_OF_INPUT:
    printError("Syntax Error", error.lineCount, error.charCount, "Unexpected end-of-input token");
    break;
  case UNEXPECTED_CHARACTER:
    printError("Syntax Error", error.lineCount, error.charCount, "Unexpected character");
    break;
  }

  printTokens(manager);

  deleteTokenManager(manager);
  fclose(jsonFile);

  return 0;
}
