#include "my_lang_lib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void nameDtor(struct Name *name) {

    assert(name);

    char *ptr = const_cast<char *>(name->name);
    free(ptr);
    free(name);
}

const char *getStrPtr(Vector *names_table, size_t index) {

    assert(names_table);

    return ((Name *) getPtr(names_table, index))->name;
}

NameType getNameType(Vector *names_table, size_t index) {

    assert(names_table);

    return ((Name *) getPtr(names_table, index))->type;
}

void namesDtor(Vector *names_table) {

    assert(names_table);

    for (size_t i = NUMBER_OF_KEY_WORDS; i < names_table->size; i++) {
        char *ptr = const_cast<char *>(((Name *) getPtr(names_table, i))->name);
        free(ptr);
        ((Name *) getPtr(names_table, i))->name = NULL;
    }
}

void tokensDtor(Vector *tokens) {

    assert(tokens);

    for (size_t i = 0; i < tokens->size; i++) {
        if (((Token *) getPtr(tokens, i))->type == STRING) {
            free(((Token *) getPtr(tokens, i))->string);
        }
    }

    vectorDtor(tokens);
}