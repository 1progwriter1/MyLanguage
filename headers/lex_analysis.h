#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include "my_vector.h"
#include "system_words.h"
#include "names_table.h"

int LexicalAnalysis(NamesTable *data, Vector *tokens, const char *filename);

#endif