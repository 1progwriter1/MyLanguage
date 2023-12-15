#ifndef NAMES_TABLE_LANG
#define NAMES_TABLE_LANG

#include <stdio.h>
#include "key_words.h"

enum NameType {
    VAR_NAME  = 0,
    KEY_WORD  = 1,
    FUNC_NAME = 2,
    STR       = 3
};

struct Name {
    const char *name;
    NameType type;
};

struct NamesTable {
    Name *names;
    size_t size;
    size_t capacity;
};

int NamesTableCtor(NamesTable *data);
int NamesTableDtor(NamesTable *data);
int PushName(NamesTable *data, Name value);
int PopName(NamesTable *data, Name *dst);

#endif
