#ifndef VECTOR_CPU
#define VECTOR_CPU

#include <stdio.h>
#include "key_words_codes.h"

struct Token {
    ValueType type;
    union {
        double number;
        Binary_Op bin_op;
        Unary_Op un_op;
        Key_Op key_op;
        size_t var_index;
        size_t func_index;
        Punctuation sym_code;
        char *string;
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