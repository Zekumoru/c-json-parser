#ifndef UTILS_H
#define UTILS_H

#include "json-parser.h"
#include <stddef.h>

/**
 * @brief Alloca o ridimensiona dinamicamente un array.
 *
 * La funzione ridimensiona un array alla capacità minima necessaria per contenere il numero di elementi specificato.
 * La nuova capacità è calcolata come la potenza di 2 successiva alla dimensione richiesta.
 * Se la riallocazione fallisce, la memoria precedente viene liberata.
 *
 * @param vec Puntatore all'array esistente o NULL per una nuova allocazione.
 * @param cap Puntatore alla variabile che conterrà la nuova capacità allocata.
 * @param size Numero di elementi che l'array deve contenere.
 * @param elemSize Dimensione in byte di un singolo elemento dell'array.
 *
 * @return Puntatore all'array ridimensionato, o NULL se l'allocazione fallisce.
 *
 * @note Se `size` o `elemSize` sono zero, la funzione restituisce il puntatore originale senza alcuna modifica.
 * @note Se la riallocazione fallisce, la memoria precedente viene liberata.
 */
void* vec_alloc(void* vec, size_t* cap, const size_t size, const size_t elemSize);

/**
 * @file vstrdup.h
 * @brief Funzione per creare una stringa terminata con null formattata secondo specifiche.
 */

/**
 * @brief Crea e restituisce una stringa terminata con null, formattata secondo le specifiche fornite.
 *
 * Questa funzione consente di formattare una stringa utilizzando specificatori di formato,
 * in modo simile a printf, e di restituirla come puntatore a una nuova stringa allocata dinamicamente.
 *
 * @param fmt Una stringa che può contenere specificatori di formato.
 * @param ... Argomenti addizionali utilizzati per i specificatori di formato nella stringa.
 * @return Un puntatore a una stringa terminata con null che deve essere liberata manualmente con `free`.
 * @warning Non dimenticare di liberare la memoria del puntatore restituito per evitare perdite di memoria.
 *
 * @note La memoria viene allocata dinamicamente, quindi il chiamante è responsabile della sua gestione.
 */
char* vstrdup(const char* fmt, ...);

/**
 * @brief Stampa un messaggio di errore con dettagli di posizione.
 *
 * @param errorType Tipo di errore come stringa.
 * @param lineCount Numero di linea dell'errore.
 * @param charCount Numero di carattere dell'errore.
 * @param message Messaggio di errore dettagliato.
 */
void printError(const char* errorType, size_t lineCount, size_t charCount, const char* message);

/**
 * @brief Stampa un errore lessicale basato su una struttura LexError.
 *
 * @param error Puntatore alla struttura contenente i dettagli dell'errore lessicale.
 */
void printLexError(LexError* error);

/**
 * @brief Stampa un errore di parsing basato su una struttura ParserError.
 *
 * @param error Puntatore alla struttura contenente i dettagli dell'errore di parsing.
 */
void printParseError(ParserError* error);

/**
 * @brief Stampa tutti i token presenti nella struttura TokenManager.
 *
 * @param manager Puntatore alla struttura contenente l'array di token.
 */
void printTokens(TokenManager* manager);

/**
 * @brief Stampa una stringa con indentazione specificata.
 *
 * @param indent Livello di indentazione in spazi.
 * @param fmt Formato della stringa da stampare (simile a printf).
 * @param ... Argomenti variabili per la formattazione della stringa.
 */
void printWithIndent(size_t indent, const char* fmt, ...);

/**
 * @brief Attraversa ricorsivamente un nodo JSON e ne stampa il contenuto.
 *
 * @param node Puntatore al nodo JSON da attraversare.
 * @param indent Livello di indentazione per la stampa.
 * @param isArrayNode Indica se il nodo attuale appartiene a un array.
 */
void traverse(JsonNode* node, size_t indent, bool isArrayNode);

#endif // UTILS_H
