#include "json-parser.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

JsonNode* createJsonNode(JsonNodeType type)
{
  JsonNode* node = (JsonNode*)malloc(sizeof(JsonNode));
  node->type = type;
  node->key = NULL;
  node->value.v_object = NULL;
  node->isRoot = false;
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

JsonNode* parse_helper(FILE* jsonFile, TokenManager* manager, ParserError* error)
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
    return parseInteger(jsonFile, token, error);
  if (token->type == DOUBLE_LEX)
    return parseDouble(jsonFile, token, error);
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

JsonNode* parse(FILE* jsonFile, TokenManager* manager, ParserError* error)
{
  if (error)
    error->type = NO_PARSER_ERROR;
  JsonNode* root = parse_helper(jsonFile, manager, error);
  root->isRoot = true;
  return root;
}

void addObjectPair(JsonNode* node, JsonNode* pairNode)
{
  node->vSize++;
  node->value.v_object = (JsonNode*)vec_alloc(node->value.v_object, &node->vCapacity, node->vSize, sizeof(JsonNode));
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
    JsonNode* valueNode = parse_helper(jsonFile, manager, error);
    valueNode->key = pairKey;
    if (valueNode == NULL || (error && error->type != NO_PARSER_ERROR))
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
  node->value.v_array = (JsonNode*)vec_alloc(node->value.v_array, &node->vCapacity, node->vSize, sizeof(JsonNode));
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

  manager->pos--;
  while (true)
  {
    JsonNode* elemNode = parse_helper(jsonFile, manager, error);
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

char* getStringFromToken(FILE* jsonFile, Token* token)
{
  // strLength has some implicit calculations
  // +1 for '\0' and -2 for the double quotes however
  // since startPos starts after the first double quote,
  // we have +1 for '\0' and -1 for the ending double quote which is 0
  size_t strLength = token->endPos - token->startPos;
  size_t startPos = token->startPos;

  if (token->type != STRING_LEX)
    strLength++;
  else
    startPos++; // skips the first double quotes in a string token

  char* str = (char*)malloc(strLength);

  fseek(jsonFile, startPos, SEEK_SET);
  fgets(str, strLength, jsonFile);

  return str;
}

JsonNode* parseString(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(STRING_NODE);
  node->value.v_string = getStringFromToken(jsonFile, token);
  return node;
}

JsonNode* parseInteger(FILE* jsonFile, Token* token, ParserError* error)
{
  JsonNode* node = createJsonNode(INTEGER_NODE);

  char* input = getStringFromToken(jsonFile, token);

  char* endptr;
  node->value.v_int = (int)strtol(input, &endptr, 10);

  if (*endptr != '\0' && error)
  {
    error->type = INVALID_INTEGER_LITERAL;
    error->token = *token;
  }

  free(input);
  return node;
}

JsonNode* parseDouble(FILE* jsonFile, Token* token, ParserError* error)
{
  JsonNode* node = createJsonNode(DOUBLE_NODE);

  char* input = getStringFromToken(jsonFile, token);

  char* endptr;
  node->value.v_double = strtod(input, &endptr);

  if (*endptr != '\0' && error)
  {
    error->type = INVALID_DOUBLE_LITERAL;
    error->token = *token;
  }

  free(input);
  return node;
}

JsonNode* parseBoolean(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(BOOLEAN_NODE);

  fseek(jsonFile, token->startPos, SEEK_SET);
  int c = fgetc(jsonFile);

  node->value.v_bool = (c == 't');

  return node;
}

JsonNode* parseNull(FILE* jsonFile, Token* token)
{
  JsonNode* node = createJsonNode(NULL_NODE);
  return node;
}

void freeJsonTree(JsonNode* node)
{
  if (node == NULL)
    return;

  if (node->key != NULL)
    free(node->key);

  switch (node->type)
  {
  case NULL_NODE:
  case INTEGER_NODE:
  case DOUBLE_NODE:
  case BOOLEAN_NODE:
    break; // freed below
  case STRING_NODE:
    free(node->value.v_string);
    break;
  case OBJECT_NODE:
  case ARRAY_NODE:
    JsonNode* nodeList;
    if (node->type == OBJECT_NODE)
    {
      nodeList = node->value.v_object;
    }
    else
    {
      nodeList = node->value.v_array;
    }

    for (size_t i = 0; i < node->vSize; i++)
      freeJsonTree(&nodeList[i]);

    free(nodeList);
    break;
  }

  // Free root node which is an OBJECT node with no key
  // As for the other nodes, they're freed together above
  // inside the free(nodeList)
  if (node->isRoot)
    free(node);
}
