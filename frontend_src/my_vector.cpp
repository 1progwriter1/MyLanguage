#include <stdio.h>
#include "../headers/my_vector.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"

const int INCREASE = 2;
const int MIN_SIZE = 8;

enum VecChangeSize {
    ChangeSizeRaise = 0,
    ChageSizeCut    = 1,
};

static int Resize(Vector *vec, VecChangeSize action);
static int VerifyVector(Vector *vec);

int VectorCtor(Vector *vec, size_t size) {

    assert(vec);

    vec->data = (Vec_t *) calloc (size, sizeof (Vec_t));
    if (!vec->data)
        return NO_MEMORY;

    vec->size = 0;
    vec->capacity = size;

    return SUCCESS;
}

int VectorDtor(Vector *vec) {

    assert(vec);

    free(vec->data);
    vec->size = 0;
    vec->capacity = 0;

    return SUCCESS;
}

int PushBack(Vector *vec, Vec_t num) {

    assert(vec);

    if (VerifyVector(vec) != SUCCESS)
        return ERROR;

    if (vec->size >= vec->capacity) {
        if (Resize(vec, ChangeSizeRaise) != SUCCESS)
            return NO_MEMORY;
    }

    vec->data[vec->size++] = num;

    return SUCCESS;
}

int Pop(Vector *vec, Vec_t *num) {

    assert(vec);

    if (VerifyVector(vec) != SUCCESS)
        return ERROR;

    if (vec->size - 1 < 0) {
        printf(RED "Vector is empty. Unable to pop value" END_OF_COLOR "\n");
        return NULL;
    }

    *num = vec->data[--vec->size];
    if (vec->capacity > MIN_SIZE && vec->capacity - vec->size > vec->capacity - vec->capacity / INCREASE) {
        if (Resize(vec, ChageSizeCut) != SUCCESS)
            return NO_MEMORY;
    }
    return SUCCESS;

}
static int Resize(Vector *vec, VecChangeSize action) {

    assert(vec);

    if (action == ChangeSizeRaise) {
        vec->capacity *= INCREASE;
    }
    else {
        vec->capacity /= INCREASE;
    }

    vec->data = (Vec_t *) realloc (vec->data, sizeof (Vec_t) * vec->capacity);
    if (!vec->data) {
        printf(RED "Vector error: " END_OF_COLOR "memory allocation error\n");
        return NO_MEMORY;
    }

    for (size_t i = vec->size; i < vec->capacity; i++) {
        vec->data[i] = (Token) {};
    }
    return SUCCESS;
}

static int VerifyVector(Vector *vec) {

    assert(vec);

    if (!vec->data) {
        printf(RED "Vector verifying: " END_OF_COLOR "NULL data\n");
        return ERROR;
    }
    if (vec->size > vec->capacity) {
        printf(RED "Vector verifying: " END_OF_COLOR "incorrect size or capacity\n");
        return ERROR;
    }

    return SUCCESS;
}