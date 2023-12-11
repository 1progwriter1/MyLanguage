#ifndef NAMES_TABLE_LANG
#define NAMES_TABLE_LANG

#include <stdio.h>

enum NameType {
    VAR   = 0,
    FUNC  = 1,
    SYS_W = 2,
};

struct Name {
    char *name;
    NameType typr;
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
