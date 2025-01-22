#ifndef JSON_PARSER_C
#define JSON_PARSER_C

#include <stddef.h>
#include <stdio.h>

/**
 * LEXICAL ANALYSIS
 */

typedef enum TokenType
{
  NULL_LEX = 0,
  STRING_LEX,
  INTEGER_LEX,
  DOUBLE_LEX,
  BOOLEAN_LEX,
  CURLY_OPEN = '{',
  CURLY_CLOSE = '}',
  BRACKET_OPEN = '[',
  BRACKET_CLOSE = ']',
  COMMA = ',',
  COLON = ':'
} TokenType;

typedef struct Token
{
  TokenType type;
  size_t startPos;
  size_t endPos;
  // For parser error
  size_t lineCount;
  size_t charCount;
} Token;

typedef struct TokenManager
{
  Token* tokens;
  size_t capacity;
  size_t size;
  size_t pos;
} TokenManager;

TokenManager* createTokenManager();

void deleteTokenManager(TokenManager* manager);

Token* createToken(TokenManager* manager);

typedef enum LexErrorType
{
  NO_LEX_ERROR = 0,
  EXPECTED_END_OF_STRING,
  INVALID_BOOLEAN_LITERAL,
  INVALID_NULL_LITERAL,
  UNEXPECTED_CHARACTER,
  UNEXPECTED_END_OF_INPUT
} LexErrorType;

typedef struct LexError
{
  LexErrorType type;
  size_t lineCount;
  size_t charCount;
} LexError;

TokenManager* lex(FILE* jsonFile, LexError* error);

/**
 * SYNTACTIC ANALYSIS
 */

typedef enum JsonNodeType
{
  NULL_NODE = 0,
  OBJECT_NODE,
  ARRAY_NODE,
  STRING_NODE,
  INTEGER_NODE,
  DOUBLE_NODE,
  BOOLEAN_NODE
} JsonNodeType;

typedef union JsonValue
{
  struct JsonNode* v_object;
  struct JsonNode* v_array;
  char* v_string;
  int v_int;
  double v_double;
  bool v_bool;
} JsonValue;

typedef struct JsonNode
{
  JsonNodeType type;
  char* key;
  JsonValue value;
  // If the value happens to be an array or object then it
  // needs these dynamic allocation variables
  size_t vCapacity;
  size_t vSize;
} JsonNode;

JsonNode* createJsonNode(JsonNodeType type);

typedef enum ParserErrorType
{
  NO_PARSER_ERROR = 0,
  INVALID_INTEGER_LITERAL,
  INVALID_DOUBLE_LITERAL,
  EXPECTED_OBJECT_KEY,
  EXPECTED_END_OF_OBJECT_BRACE,
  EXPECTED_END_OF_ARRAY_BRACE,
  EXPECTED_COLON,
  EXPECTED_COMMA,
  UNEXPECTED_TOKEN,
} ParserErrorType;

typedef struct ParserError
{
  ParserErrorType type;
  Token token;
} ParserError;

Token* advance(TokenManager* manager);

void addObjectPair(JsonNode* node, JsonNode* pairNode);
void addElement(JsonNode* node, JsonNode* elemNode);

JsonNode* parseObject(FILE* jsonFile, TokenManager* manager, ParserError* error);
JsonNode* parseArray(FILE* jsonFile, TokenManager* manager, ParserError* error);
JsonNode* parseString(FILE* jsonFile, Token* token);
JsonNode* parseInteger(FILE* jsonFile, Token* token, ParserError* error);
JsonNode* parseDouble(FILE* jsonFile, Token* token, ParserError* error);
JsonNode* parseBoolean(FILE* jsonFile, Token* token);
JsonNode* parseNull(FILE* jsonFile, Token* token);

JsonNode* parse(FILE* jsonFile, TokenManager* manager, ParserError* error);

void freeJsonTree(JsonNode* node);

#endif // JSON_PARSER_C
