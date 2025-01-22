#include "json-parser.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

TokenManager* createTokenManager()
{
  TokenManager* manager = (TokenManager*)malloc(sizeof(TokenManager));
  manager->tokens = NULL;
  manager->capacity = 0;
  manager->size = 0;
  manager->pos = 0;
  return manager;
}

void deleteTokenManager(TokenManager* manager)
{
  free(manager->tokens);
  free(manager);
}

Token* createToken(TokenManager* manager)
{
  manager->size++;
  manager->tokens = (Token*)valloc(manager->tokens, &manager->capacity, manager->size, sizeof(Token));
  return &manager->tokens[manager->size - 1];
}

bool matchFileCharacter(FILE* jsonFile, int match, LexError* error, LexErrorType errorType)
{
  int c = fgetc(jsonFile);
  if ((c != match || c == EOF) && error)
  {
    error->type = errorType;
    return false;
  }
  return true;
}

TokenManager* lex(FILE* jsonFile, LexError* error)
{
  fseek(jsonFile, 0, SEEK_SET);
  if (error)
    error->type = NO_LEX_ERROR;

  TokenManager* manager = createTokenManager();

  int c;
  size_t lineCount = 0;
  size_t charCount = 0;
  while ((c = fgetc(jsonFile)) != EOF)
  {
    charCount++;

    if (c == '\n' || c == '\r')
    {
      lineCount++;
      charCount = 0;

      // Handle possible Windows newline by ignoring it's adjacent \n
      if (c == '\r')
      {
        int next = fgetc(jsonFile);
        if (next != '\n' && next != EOF)
          ungetc(next, jsonFile);
      }
    }

    if (isspace(c))
    {
      continue;
    }

    Token* token = createToken(manager);
    token->startPos = ftell(jsonFile) - 1;

    error->lineCount = lineCount + 1;
    error->charCount = charCount;

    switch (c)
    {
    case '{':
    case '}':
    case '[':
    case ']':
    case ',':
    case ':':
      token->type = (TokenType)c;
      token->endPos = token->startPos;
      continue;
    }

    if (c == '"')
    {
      token->type = STRING_LEX;

      do
      {
        c = fgetc(jsonFile);
        charCount++;
      } while (c != '"' && c != EOF);

      if (c == '"')
        token->endPos = ftell(jsonFile) - 1;

      if (c == EOF && error)
      {
        error->type = EXPECTED_END_OF_STRING;
        return manager;
      }
    }
    else if (c == '-' || isdigit(c))
    {
      bool isDouble = false;
      do
      {
        c = fgetc(jsonFile);
        if (c == '.')
          isDouble = true;
        charCount++;
      } while ((isdigit(c) || c == '.') && c != EOF);
      ungetc(c, jsonFile);

      if (isDouble)
        token->type = DOUBLE_LEX;
      else
        token->type = INTEGER_LEX;

      if (c == EOF && error)
      {
        error->type = UNEXPECTED_END_OF_INPUT;
        return manager;
      }
    }
    else if (c == 't')
    {
      token->type = BOOLEAN_LEX;

      if (!matchFileCharacter(jsonFile, 'r', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'u', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'e', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      charCount += 3;
    }
    else if (c == 'f')
    {
      token->type = BOOLEAN_LEX;

      if (!matchFileCharacter(jsonFile, 'a', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'l', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 's', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'e', error, INVALID_BOOLEAN_LITERAL))
        return manager;
      charCount += 4;
    }
    else if (c == 'n')
    {
      token->type = NULL_LEX;

      if (!matchFileCharacter(jsonFile, 'u', error, INVALID_NULL_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'l', error, INVALID_NULL_LITERAL))
        return manager;
      if (!matchFileCharacter(jsonFile, 'l', error, INVALID_NULL_LITERAL))
        return manager;
      charCount += 3;
    }
    else
    {
      error->type = UNEXPECTED_CHARACTER;
      return manager;
    }
  }

  return manager;
}