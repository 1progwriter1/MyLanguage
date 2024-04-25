#ifndef DUMP_LEXICAL_ANALYSIS
#define DUMP_LEXICAL_ANALYSIS

#include "../../MyLibraries/headers/my_vector.h"

void dumpErrors(StringParseData *data, Vector *tokens, Vector *names_table);

void dumpAll(Vector *tokens, Vector *names_table, const char *filename);

#endif