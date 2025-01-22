#include "json-parser.h"

Token* advance(TokenManager* manager)
{
  if (manager->pos >= manager->size)
    return NULL;

  Token* token = manager->tokens + manager->pos;
  manager->pos++;
  return token;
}

JSONNode* parse(TokenManager* manager, ParserError* error)
{
  if (error && error->type != NO_PARSER_ERROR)
    return NULL;

  Token* token = advance(manager);

  if (token->type == CURLY_OPEN)
    return parseObject(manager);
  if (token->type == BRACKET_OPEN)
    return parseArray(manager);
  if (token->type == STRING_LEX)
    return parseString(token);
  if (token->type == INTEGER_LEX)
    return parseInteger(token);
  if (token->type == DOUBLE_LEX)
    return parseDouble(token);
  if (token->type == BOOLEAN_LEX)
    return parseBoolean(token);
  if (token->type == NULL_LEX)
    return parseNull(token);

  if (error)
  {
    error->type = UNEXPECTED_TOKEN;
    error->token = *token; // duplicate token in stack
    return NULL;
  }
}
