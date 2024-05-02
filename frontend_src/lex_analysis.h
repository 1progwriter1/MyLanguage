#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include "../headers/key_words_codes.h"
#include "../../MyLibraries/headers/my_vector.h"

enum NameType {
    VAR_NAME  = 0,
    KEY_WORD  = 1,
    FUNC_NAME = 2,
    STR       = 3,
};

struct Name {
    const char *name;
    NameType type;
};

int analyzeLexis(Vector *names_table, Vector *tokens, const char *filename);

void nameDtor(Name *name);
void namesDtor(Vector *names_table);

NameType    getNameType(Vector *names_table, size_t index);
const char *getStrPtr  (Vector *data, size_t index);

#endif