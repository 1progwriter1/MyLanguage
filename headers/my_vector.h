#ifndef VECTOR_CPU
#define VECTOR_CPU

#include <stdio.h>
#include "system_words.h"

struct Token {
    TokenType type;
    union {
        double number;
        TokenOperation operation;
        char var;
        SWCodes sys_word;
    };
};

typedef Token Vec_t;

struct Vector {
    Vec_t *data;
    size_t size;
    size_t capacity;
};

int VectorCtor(Vector *vec, size_t size);
int VectorDtor(Vector *vec);
int PushBack(Vector *vec, Vec_t num);
int Pop(Vector *vec, Vec_t *num);

#endif