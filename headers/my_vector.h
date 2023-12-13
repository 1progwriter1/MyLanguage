#ifndef VECTOR_CPU
#define VECTOR_CPU

#include <stdio.h>
#include "system_words.h"

enum TokenValueType {
    TOKEN_KEY_WORD  = 0,
    TOKEN_NUMBER    = 1,
    TOKEN_VARIABLE  = 2,
    TOKE_FUNCTION   = 3,
    TOKEN_PUNCT_SYM = 4
};

struct Token {
    TokenValueType type;
    union {
        double number;
        size_t key_word;
        size_t var_index;
        size_t func_index;
        Punctuation code;
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