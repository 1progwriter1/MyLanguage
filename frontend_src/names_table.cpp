#include <stdio.h>
#include "../headers/names_table.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"

enum ResizeOptions {
    kResizeRaise = 0,
    kResizeCut   = 1,
};

const size_t INCREASE = 2;
const size_t START_NUM_OF_NAMES = 8;

static int NamesTableVerify(NamesTable *data);
static int NamesTableResize(NamesTable *data, const ResizeOptions option);

int NamesTableCtor(NamesTable *data, const char *KEY_WORDS) {

    assert(data);

    data->names = (Name *) calloc (START_NUM_OF_NAMES, sizeof (Name));
    if (!data->names) return NO_MEMORY;

    data->capacity = START_NUM_OF_NAMES;
    data->size = 0;

    return SUCCESS;
}

int NamesTableDtor(NamesTable *data) {

    assert(data);

    free(data->names);
    data->names = NULL;

    data->size = 0;
    data->capacity = 0;

    return SUCCESS;
}

int PushName(NamesTable *data, Name value) {

    assert(data);

    if (NamesTableVerify(data) != SUCCESS)
        return ERROR;

    if (data->size >= data->capacity)
        if (NamesTableResize(data, kResizeRaise) != SUCCESS)
            return ERROR;

    data->names[data->size++] = value;

    return SUCCESS;
}

int PopName(NamesTable *data, Name *dst) {

    assert(data);
    assert(dst);

    if (NamesTableVerify(data) != SUCCESS)
        return ERROR;

    *dst = data->names[--data->size - 1];

    if (data->capacity > START_NUM_OF_NAMES && data->capacity - data->size > data->capacity - data->capacity / INCREASE) {
        if (NamesTableResize(data, kResizeCut) != SUCCESS)
            return NO_MEMORY;
    }

    return SUCCESS;

}

static int NamesTableResize(NamesTable *data, const ResizeOptions option) {

    assert(data);

    if (NamesTableVerify(data) != SUCCESS)
        return ERROR;

    const size_t old_capacity = data->capacity;

    if (option == kResizeRaise) {
        data->capacity *= INCREASE;
    }
    else if (option == kResizeCut) {
        data->capacity /= INCREASE;
    }
    else {
        printf(RED "Invalid resize option" END_OF_COLOR "\n");
        return ERROR;
    }

    data->names = (Name *) realloc (data->names, data->capacity * sizeof (Name));
    if (!data->names) return NO_MEMORY;

    for (size_t i = old_capacity; i < data->capacity; i++)
        data->names[i] = {};

    return SUCCESS;
}

static int NamesTableVerify(NamesTable *data) {

    assert(data);

    bool error = false;

    if (!data->names) {
        printf(RED "NULL pointer names" END_OF_COLOR "\n");
        error = true;
    }

    if (data->size > data->capacity) {
        printf(RED "Incorrect size or capacity" END_OF_COLOR "\n");
        error = true;
    }

    if (error)
        return ERROR;

    return SUCCESS;
}