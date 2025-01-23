/**
 * JSON Parser in C
 *
 * Questo programma implementa un parser JSON scritto in linguaggio C,
 * che esegue sia l'analisi lessicale che sintattica di un file JSON.
 *
 * A parte la documentazione, NON HO UTILIZZATO alcun strumento di
 * intelligenza artificiale per scrivere nessuna parte del codice.
 * Tutto il codice è stato scritto INTERAMENTE da me.
 *
 * Per quanto riguarda la logica del parser, ho letto tre articoli
 * online che implementano un proprio parser JSON in C++, Python e
 * JavaScript.Ho seguito maggiormente quello in Python.
 * Inoltre, gli articoli non gestivano bene gli errori, quindi
 * ho dovuto elaborare da solo un approccio alla gestione degli errori.
 *
 * Ho già un po' di esperienza nella scrittura di parser semplici,
 * il che mi ha aiutato molto.
 *
 * Per gli articoli citati:
 * - Writing a simple JSON parser (https://notes.eatonphil.com/writing-a-simple-json-parser.html)
 * - Building a JSON Parser from scratch with JS (https://dev.to/vit0rr/building-a-json-parser-from-scratch-with-js-3180)
 * - Writing a simple JSON Parser from scratch in C++ (https://kishoreganesh.com/post/writing-a-json-parser-in-cplusplus)
 *
 * Funzionalità principali
 * - Lettura di un file JSON di input.
 * - Analisi lessicale per generare una sequenza di token a partire
 *   dal contenuto JSON.
 * - Analisi sintattica per costruire una struttura ad albero del
 *   documento JSON.
 * - Segnalazione di eventuali errori lessicali e sintattici individuati
 *   durante il processo.
 * - Visualizzazione strutturata (simile a yaml) del contenuto JSON analizzato.
 *
 * Componenti del programma
 * - Analizzatore Lessicale (Lexer):
 *   - Effettua la scansione del file JSON, identificando token come stringhe,
 *     numeri, ecc.
 *   - Gestisce eventuali errori di sintassi lessicale.
 *
 * - Analizzatore Sintattico (Parser):
 *   - Costruisce un albero sintattico a partire dai token generati.
 *   - Identifica errori di sintassi.
 *
 * - Funzioni di stampa e supporto:
 *   - Output dettagliato dei token rilevati.
 *   - Stampa strutturata del contenuto JSON in formato simile a yaml
 *     per facilitare la visualizzazione.
 *   - Gestione della memoria allocata per i token e l'albero sintattico.
 *
 * Esempio di utilizzo
 * - Il programma legge un file JSON denominato `sample.json`, esegue l'analisi
 *   e stampa i risultati della tokenizzazione e dell'analisi sintattica.
 */

#include "app/json-parser.h"
#include "app/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
  FILE* jsonFile = fopen("sample.json", "r");

  if (!jsonFile)
  {
    perror("Could not open json file.");
    exit(1);
  }

  LexError lexError;
  TokenManager* manager = lex(jsonFile, &lexError);

  if (lexError.type != NO_LEX_ERROR)
  {
    printLexError(&lexError);
    exit(2);
  }

  printf("LEXICAL ANALYSIS\n");
  printTokens(manager);
  printf("\n");

  ParserError parserError;
  JsonNode* root = parse(jsonFile, manager, &parserError);

  if (parserError.type != NO_PARSER_ERROR)
  {
    printParseError(&parserError);
    exit(3);
  }

  printf("SYNTACTIC ANALYSIS\n");
  traverse(root, 0, false);

  freeJsonTree(root);
  deleteTokenManager(manager);
  fclose(jsonFile);

  return 0;
}
