#ifndef MY_LANGUAGE_LIBRARY
#define MY_LANGUAGE_LIBRARY

#include "../headers/key_words_codes.h"
#include "../../MyLibraries/headers/my_vector.h"

const char *getStrPtr(Vector *names_table, size_t index);
void nameDtor(Name *name);
NameType getNameType(Vector *names_table, size_t index);
void namesDtor(Vector *names_table);

#endif