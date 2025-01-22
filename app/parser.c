#include "json-parser.h"
#include "utils.h"
#include <stdbool.h>
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

JsonNode* parse(FILE* jsonFile, TokenManager* manager, ParserError* error)
{
  if (error && error->type != NO_PARSER_ERROR)
    return NULL;

  Token* token = advance(manager);
  if (token == NULL)
    return NULL;

  if (token->type == CURLY_OPEN)
    return parseObject(jsonFile, manager, error);
  if (token->type == BRACKET_OPEN)
    return parseArray(jsonFile, manager, error);
  if (token->type == STRING_LEX)
    return parseString(jsonFile, token);
  if (token->type == INTEGER_LEX)
    return parseInteger(jsonFile, token);
  if (token->type == DOUBLE_LEX)
    return parseDouble(jsonFile, token);
  if (token->type == BOOLEAN_LEX)
    return parseBoolean(jsonFile, token);
  if (token->type == NULL_LEX)
    return parseNull(jsonFile, token);

  if (error)
  {
    error->type = UNEXPECTED_TOKEN;
    error->token = *token; // duplicate token in stack
  }
  return NULL;
}

void addObjectPair(JsonNode* node, JsonNode* pairNode)
{
  node->vSize++;
  node->value.v_object = (JsonNode*)valloc(node->value.v_object, &node->vCapacity, node->vSize, sizeof(JsonNode));
  node->value.v_object[node->vSize - 1] = *pairNode;
}

JsonNode* parseObject(FILE* jsonFile, TokenManager* manager, ParserError* error)
{
  JsonNode* node = createJsonNode(OBJECT_NODE);

  Token* token = advance(manager);
  if (token == NULL)
  {
    if (error)
      error->type = EXPECTED_END_OF_OBJECT_BRACE;
    return node;
  }

  // Handle empty object {}
  if (token->type == CURLY_CLOSE)
    return node;

  manager->pos--;
  while (true)
  {
    token = advance(manager);

    // Get object's pair key
    if (token == NULL || token->type != STRING_LEX)
    {
      if (error)
      {
        error->type = EXPECTED_OBJECT_KEY;
        if (token != NULL)
          error->token = *token;
      }
      return node;
    }

    JsonNode* strNode = parseString(jsonFile, token);
    char* pairKey = strNode->value.v_string;
    free(strNode);

    token = advance(manager);
    if (token == NULL || token->type != COLON)
    {
      if (error)
      {
        error->type = EXPECTED_COLON;
        if (token != NULL)
          error->token = *token;
      }
      free(pairKey);
      return node;
    }

    // Get object's pair value
    JsonNode* valueNode = parse(jsonFile, manager, error);
    valueNode->key = pairKey;
    if (valueNode == NULL && error && error->type != NO_PARSER_ERROR)
      return node;
    addObjectPair(node, valueNode);
    free(valueNode);

    token = advance(manager);
    if (token == NULL)
    {
      if (error)
        error->type = EXPECTED_END_OF_OBJECT_BRACE;
      return node;
    }

    if (token->type == CURLY_CLOSE)
      return node;

    if (token->type != COMMA)
    {
      if (error)
      {
        error->type = EXPECTED_COMMA;
        error->token = *token;
      }
      return node;
    }
  }

  if (error)
  {
    error->type = EXPECTED_END_OF_OBJECT_BRACE;
    error->token = *token;
  }
  return node;
}

void addElement(JsonNode* node, JsonNode* elemNode)
{
  node->vSize++;
  node->value.v_array = (JsonNode*)valloc(node->value.v_array, &node->vCapacity, node->vSize, sizeof(JsonNode));
  node->value.v_array[node->vSize - 1] = *elemNode;
}

JsonNode* parseArray(FILE* jsonFile, TokenManager* manager, ParserError* error)
{
  JsonNode* node = createJsonNode(ARRAY_NODE);

  Token* token = advance(manager);
  if (token == NULL)
  {
    if (error)
      error->type = EXPECTED_END_OF_ARRAY_BRACE;
    return node;
  }

  // Handle empty array []
  if (token->type == BRACKET_CLOSE)
    return node;

  while (true)
  {
    JsonNode* elemNode = parse(jsonFile, manager, error);
    if (elemNode == NULL && error && error->type != NO_PARSER_ERROR)
      return node;
    addElement(node, elemNode);
    free(elemNode);

    token = advance(manager);
    if (token == NULL)
    {
      if (error)
        error->type = EXPECTED_END_OF_ARRAY_BRACE;
      return node;
    }

    if (token->type == BRACKET_CLOSE)
      return node;

    if (token->type != COMMA)
    {
      if (error)
      {
        error->type = EXPECTED_COMMA;
        error->token = *token;
      }
      return node;
    }
  }

  if (error)
  {
    error->type = EXPECTED_END_OF_ARRAY_BRACE;
    error->token = *token;
  }
  return node;
}

JsonNode* parseString(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(STRING_NODE);

  // strLength has some implicit calculations
  // +1 for '\0' and -2 for the double quotes however
  // since startPos starts after the first double quote,
  // we have +1 for '\0' and -1 for the ending double quote which is 0
  size_t strLength = token->endPos - token->startPos;
  node->value.v_string = (char*)malloc(strLength);
  fseek(jsonFile, token->startPos + 1, SEEK_SET);
  fgets(node->value.v_string, strLength, jsonFile);

  return node;
}

JsonNode* parseInteger(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(INTEGER_NODE);
  return node;
}

JsonNode* parseDouble(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(DOUBLE_NODE);
  return node;
}

JsonNode* parseBoolean(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(BOOLEAN_NODE);
  return node;
}

JsonNode* parseNull(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(NULL_NODE);
  return node;
}
