#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include "my_vector.h"
#include "key_words_codes.h"
#include "names_table.h"

#define SRC_FILE "text.txt"

int LexicalAnalysis(NamesTable *data, Vector *tokens, const char *filename);

#endif