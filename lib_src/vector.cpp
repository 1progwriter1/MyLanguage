#include <stdio.h>
#include "my_vector.h"
#include <assert.h>
#include "systemdata.h"
#include <string.h>
#include <stdlib.h>

const int INCREASE_COEFF  = 2;
const int VECTOR_MIN_SIZE = 8;

enum VecChangeSize {
    ChangeSizeRaise = 0,
    ChangeSizeCut   = 1,
};

static int resize(Vector *vec, VecChangeSize action);
static int verifyVector(Vector *vec);

int vectorCtor(Vector *vec, size_t size, size_t element_size) {

    assert(vec);

    vec->data = calloc (size, element_size);
    if (!vec->data) return NO_MEMORY;

    vec->size = 0;
    vec->capacity = size;
    vec->element_size = element_size;

    return SUCCESS;
}

int vectorDtor(Vector *vec) {

    assert(vec);

    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->element_size = 0;

    return SUCCESS;
}

int pushBack(Vector *vec, void *buffer) {

    assert(vec);

    if (verifyVector(vec) != SUCCESS)
        return ERROR;

    if (vec->size >= vec->capacity) {
        if (resize(vec, ChangeSizeRaise) != SUCCESS)
            return NO_MEMORY;
    }

    memcpy((char *) vec->data + vec->size++ * vec->element_size, buffer, vec->element_size);

    return SUCCESS;
}

int pop(Vector *vec, void *buffer) {

    assert(vec);

    if (verifyVector(vec) != SUCCESS)
        return ERROR;

    if (vec->size == 0) {
        printf(RED "vector error: " END_OF_COLOR "empty. Unable to pop value\n");
        return ERROR;
    }

    memcpy(buffer, (char *) vec->data + --vec->size * vec->element_size, vec->element_size);

    if (vec->capacity > VECTOR_MIN_SIZE && vec->capacity - vec->size > vec->capacity - vec->capacity / INCREASE_COEFF) {
        if (resize(vec, ChangeSizeCut) != SUCCESS)
            return NO_MEMORY;
    }

    return SUCCESS;

}
static int resize(Vector *vec, VecChangeSize action) {

    assert(vec);

    if (action == ChangeSizeRaise) {
        vec->capacity *= INCREASE_COEFF;
    }
    else {
        vec->capacity /= INCREASE_COEFF;
    }

    void *tmp = realloc (vec->data, vec->element_size * vec->capacity);
    if (!tmp)   return NO_MEMORY;

    vec->data = tmp;

    memset((char *) vec->data + vec->size * vec->element_size, 0, (vec->capacity - vec->size) * vec->element_size);

    return SUCCESS;
}

static int verifyVector(Vector *vec) {

    assert(vec);

    if (!vec->data) {
        printf(RED "vector error: " END_OF_COLOR "null data pointer\n");
        return ERROR;
    }
    if (vec->size > vec->capacity) {
        printf(RED "vector error: " END_OF_COLOR "incorrect size or capacity\n");
        return ERROR;
    }

    return SUCCESS;
}

void *getPtr(Vector *vec, size_t index) {

    assert(vec);

    return ((char *) vec->data + index * vec->element_size);
}
