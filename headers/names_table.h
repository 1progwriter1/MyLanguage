#ifndef NAMES_TABLE_LANG
#define NAMES_TABLE_LANG

#include <stdio.h>

const size_t NUMBER_OF_KEY_WORDS = 24;

enum NameType {
    VARIABLE = 0,
    KEY_WORD = 1,
    FUNCTION = 2,
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

int NamesTableCtor(NamesTable *data, const char *KEY_WORDS[]);
int NamesTableDtor(NamesTable *data);
int PushName(NamesTable *data, Name value);
int PopName(NamesTable *data, Name *dst);

#endif
