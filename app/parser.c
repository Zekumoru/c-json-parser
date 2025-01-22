#include "json-parser.h"
#include <stdlib.h>

JsonNode* createJsonNode(JsonNodeType type)
{
  JsonNode* node = (JsonNode*)malloc(sizeof(JsonNode));
  node->type = type;
  node->key = NULL;
  node->value.v_object = NULL;
  node->vCapacity = 0;
  node->vSize = 0;
  return node;
}

Token* advance(TokenManager* manager)
{
  if (manager->pos >= manager->size)
    return NULL;

  Token* token = manager->tokens + manager->pos;
  manager->pos++;
  return token;
}

JsonNode* parse(TokenManager* manager, ParserError* error)
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

JsonNode* parseObject(TokenManager* manager)
{
  return NULL;
}

JsonNode* parseArray(TokenManager* manager)
{
  return NULL;
}

JsonNode* parseString(Token* token)
{
  return NULL;
}

JsonNode* parseInteger(Token* token)
{
  return NULL;
}

JsonNode* parseDouble(Token* token)
{
  return NULL;
}

JsonNode* parseBoolean(Token* token)
{
  return NULL;
}

JsonNode* parseNull(Token* token)
{
  return NULL;
}
