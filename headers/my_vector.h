#ifndef VECTOR_CPU
#define VECTOR_CPU

#include <stdio.h>
#include "system_words.h"

struct Token {
    ValueType type;
    union {
        double number;
        Binary_Op binary_op;
        Unary_Op unary_op;
        size_t var_index;
        size_t func_index;
        Punctuation punct_sym;
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