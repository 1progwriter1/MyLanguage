#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include "../headers/key_words_codes.h"
#include "../../MyLibraries/headers/my_vector.h"

#define SRC_FILE "text.txt"

enum NameType {
    VAR_NAME  = 0,
    KEY_WORD  = 1,
    FUNC_NAME = 2,
    STRING    = 3,
};

struct Name {
    const char *name;
    NameType type;
};

int analyzeLexis(Vector *names_table, Vector *tokens, const char *filename);

#endif