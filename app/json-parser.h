#ifndef JSON_PARSER_C
#define JSON_PARSER_C

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/**
 * ANALISI LESSICALE
 */

/**
 * @enum TokenType
 * @brief Tipi di token riconosciuti nell'analisi lessicale del JSON.
 */
typedef enum TokenType
{
  NULL_LEX = 0,        /**< Token per il valore null */
  STRING_LEX,          /**< Token per le stringhe */
  INTEGER_LEX,         /**< Token per i numeri interi */
  DOUBLE_LEX,          /**< Token per i numeri decimali */
  BOOLEAN_LEX,         /**< Token per i valori booleani */
  CURLY_OPEN = '{',    /**< Token per parentesi graffa aperta */
  CURLY_CLOSE = '}',   /**< Token per parentesi graffa chiusa */
  BRACKET_OPEN = '[',  /**< Token per parentesi quadra aperta */
  BRACKET_CLOSE = ']', /**< Token per parentesi quadra chiusa */
  COMMA = ',',         /**< Token per la virgola */
  COLON = ':'          /**< Token per i due punti */
} TokenType;

/**
 * @struct Token
 * @brief Rappresenta un token identificato nel file JSON.
 */
typedef struct Token
{
  TokenType type;   /**< Tipo di token */
  size_t startPos;  /**< Posizione iniziale del token nel file */
  size_t endPos;    /**< Posizione finale del token nel file */
  size_t lineCount; /**< Numero di linea per il rilevamento degli errori */
  size_t charCount; /**< Numero di carattere per il rilevamento degli errori */
} Token;

/**
 * @struct TokenManager
 * @brief Struttura per la gestione della memoria dei token.
 */
typedef struct TokenManager
{
  Token* tokens;   /**< Array di token */
  size_t capacity; /**< Capacità massima dell'array */
  size_t size;     /**< Numero attuale di token */
  size_t pos;      /**< Posizione corrente per la scansione */
} TokenManager;

/**
 * @brief Crea e inizializza un nuovo TokenManager.
 * @return Puntatore alla struttura TokenManager allocata.
 */
TokenManager* createTokenManager();

/**
 * @brief Dealloca la memoria utilizzata da un TokenManager.
 * @param manager Puntatore al TokenManager da eliminare.
 */
void deleteTokenManager(TokenManager* manager);

/**
 * @brief Crea un nuovo token e lo aggiunge al TokenManager.
 * @param manager Puntatore alla struttura TokenManager.
 * @return Puntatore al token creato.
 */
Token* createToken(TokenManager* manager);

/**
 * @enum LexErrorType
 * @brief Tipi di errori lessicali che possono verificarsi durante l'analisi.
 */
typedef enum LexErrorType
{
  NO_LEX_ERROR = 0,        /**< Nessun errore rilevato */
  EMPTY_FILE,              /**< File vuoto */
  EXPECTED_END_OF_STRING,  /**< Fine stringa attesa ma non trovata */
  INVALID_BOOLEAN_LITERAL, /**< Literal booleano non valido */
  INVALID_NULL_LITERAL,    /**< Literal null non valido */
  UNEXPECTED_CHARACTER,    /**< Carattere non previsto */
  UNEXPECTED_END_OF_INPUT  /**< Fine dell'input imprevista */
} LexErrorType;

/**
 * @struct LexError
 * @brief Struttura che rappresenta un errore lessicale.
 */
typedef struct LexError
{
  LexErrorType type; /**< Tipo di errore lessicale */
  size_t lineCount;  /**< Numero di linea dell'errore */
  size_t charCount;  /**< Numero di carattere dell'errore */
} LexError;

/**
 * @brief Esegue l'analisi lessicale su un file JSON.
 * @param jsonFile Puntatore al file JSON da analizzare.
 * @param error Puntatore alla struttura di errore lessicale.
 * @return Puntatore a un TokenManager contenente i token rilevati.
 */
TokenManager* lex(FILE* jsonFile, LexError* error);

/**
 * ANALISI SINTATTICA
 */

/**
 * @enum JsonNodeType
 * @brief Tipi di nodi nell'albero JSON.
 */
typedef enum JsonNodeType
{
  NULL_NODE = 0, /**< Nodo null */
  OBJECT_NODE,   /**< Nodo oggetto */
  ARRAY_NODE,    /**< Nodo array */
  STRING_NODE,   /**< Nodo stringa */
  INTEGER_NODE,  /**< Nodo numero intero */
  DOUBLE_NODE,   /**< Nodo numero decimale */
  BOOLEAN_NODE   /**< Nodo booleano */
} JsonNodeType;

/**
 * @union JsonValue
 * @brief Valori JSON rappresentati nell'albero sintattico.
 */
typedef union JsonValue
{
  struct JsonNode* v_object; /**< Puntatore a un oggetto JSON */
  struct JsonNode* v_array;  /**< Puntatore a un array JSON */
  char* v_string;            /**< Valore stringa */
  int v_int;                 /**< Valore intero */
  double v_double;           /**< Valore decimale */
  bool v_bool;               /**< Valore booleano */
} JsonValue;

/**
 * @struct JsonNode
 * @brief Nodo nell'albero JSON.
 */
typedef struct JsonNode
{
  JsonNodeType type; /**< Tipo di nodo */
  char* key;         /**< Chiave JSON (per oggetti) */
  JsonValue value;   /**< Valore del nodo */
  bool isRoot;       /**< Indica se il nodo è la radice */
  size_t vCapacity;  /**< Capacità dinamica per array/oggetto */
  size_t vSize;      /**< Dimensione attuale */
} JsonNode;

/**
 * @brief Crea un nuovo nodo JSON.
 * @param type Tipo di nodo da creare.
 * @return Puntatore al nodo JSON creato.
 */
JsonNode* createJsonNode(JsonNodeType type);

/**
 * @enum ParserErrorType
 * @brief Tipi di errori sintattici nel parsing JSON.
 */
typedef enum ParserErrorType
{
  NO_PARSER_ERROR = 0,          /**< Nessun errore */
  NO_TOKEN_FOUND,               /**< Nessun token trovato */
  INVALID_INTEGER_LITERAL,      /**< Numero intero non valido */
  INVALID_DOUBLE_LITERAL,       /**< Numero decimale non valido */
  EXPECTED_OBJECT_KEY,          /**< Attesa chiave oggetto */
  EXPECTED_END_OF_OBJECT_BRACE, /**< Attesa parentesi graffa chiusa */
  EXPECTED_END_OF_ARRAY_BRACE,  /**< Attesa parentesi quadra chiusa */
  EXPECTED_COLON,               /**< Attesi due punti */
  EXPECTED_COMMA,               /**< Attesa virgola */
  UNEXPECTED_TOKEN              /**< Token inatteso */
} ParserErrorType;

/**
 * @struct ParserError
 * @brief Rappresenta un errore sintattico nel parsing JSON.
 */
typedef struct ParserError
{
  ParserErrorType type; /**< Tipo di errore */
  Token token;          /**< Token coinvolto nell'errore */
} ParserError;

/**
 * @brief Avanza al prossimo token nella gestione dei token.
 * @param manager Puntatore alla struttura di gestione token.
 * @return Puntatore al token successivo.
 */
Token* advance(TokenManager* manager);

/**
 * @brief Aggiunge una coppia chiave-valore a un nodo oggetto JSON.
 * @param node Puntatore al nodo oggetto JSON.
 * @param pairNode Puntatore al nodo della coppia da aggiungere.
 */
void addObjectPair(JsonNode* node, JsonNode* pairNode);

/**
 * @brief Aggiunge un elemento a un nodo array JSON.
 * @param node Puntatore al nodo array JSON.
 * @param elemNode Puntatore all'elemento da aggiungere.
 */
void addElement(JsonNode* node, JsonNode* elemNode);

/**
 * @brief Effettua il parsing di un oggetto JSON.
 * @param jsonFile Puntatore al file JSON.
 * @param manager Puntatore alla struttura di gestione token.
 * @param error Puntatore alla struttura di errore.
 * @return Puntatore al nodo JSON risultante.
 */
JsonNode* parseObject(FILE* jsonFile, TokenManager* manager, ParserError* error);

/**
 * @brief Effettua il parsing di un array JSON.
 */
JsonNode* parseArray(FILE* jsonFile, TokenManager* manager, ParserError* error);

/**
 * @brief Effettua il parsing di una stringa JSON.
 */
JsonNode* parseString(FILE* jsonFile, Token* token);

/**
 * @brief Effettua il parsing di un numero intero JSON.
 */
JsonNode* parseInteger(FILE* jsonFile, Token* token, ParserError* error);

/**
 * @brief Effettua il parsing di un numero decimale JSON.
 */
JsonNode* parseDouble(FILE* jsonFile, Token* token, ParserError* error);

/**
 * @brief Effettua il parsing di un valore booleano JSON.
 */
JsonNode* parseBoolean(FILE* jsonFile, Token* token);

/**
 * @brief Effettua il parsing di un valore null JSON.
 */
JsonNode* parseNull(FILE* jsonFile, Token* token);

/**
 * @brief Esegue il parsing completo di un file JSON.
 */
JsonNode* parse(FILE* jsonFile, TokenManager* manager, ParserError* error);

/**
 * @brief Libera la memoria allocata per un albero JSON.
 */
void freeJsonTree(JsonNode* node);

#endif // JSON_PARSER_C
