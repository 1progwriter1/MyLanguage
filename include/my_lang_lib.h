#ifndef MY_LANGUAGE_LIBRARY
#define MY_LANGUAGE_LIBRARY

#include "../data/key_words_codes.h"
#include "my_vector.h"

const char *getStrPtr(Vector *names_table, size_t index);
void nameDtor(Name *name);
NameType getNameType(Vector *names_table, size_t index);
void namesDtor(Vector *names_table);
void tokensDtor(Vector *tokens);

#endif
