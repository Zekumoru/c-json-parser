#include "json-parser.h"

Token* advance(TokenManager* manager)
{
  if (manager->pos >= manager->size)
    return NULL;

  Token* token = manager->tokens + manager->pos;
  manager->pos++;
  return token;
}
