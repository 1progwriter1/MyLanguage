#include <stdio.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../headers/vars_addresses.h"
#include <assert.h>
#include <string.h>

enum VarsAddressesChangeSize {
    ChangeSizeRaise = 0,
    ChageSizeCut    = 1,
};

const int INCREASE = 2;
const size_t INITIAL_SIZE = 8;

static int VarsAddressesResize(VarsAddresses *data, VarsAddressesChangeSize action);
static int VarsAddressesVerify(VarsAddresses *data);

int VarsAddressesCtor(VarsAddresses *data) {

    assert(data);

    data->data = (Address *) calloc (INITIAL_SIZE, sizeof (Address));
    if (!data->data)
        return NO_MEMORY;

    data->size = 0;
    data->capacity = INITIAL_SIZE;

    return SUCCESS;
}

int VarsAddressesDtor(VarsAddresses *data) {

    assert(data);

    // free(data->data);
    data->data = NULL;
    data->size = 0;
    data->capacity = 0;

    return SUCCESS;
}

int PushVarAddress(VarsAddresses *data, Address value) {

    assert(data);

    if (VarsAddressesVerify(data) != SUCCESS)
        return ERROR;

    if (data->size >= data->capacity) {
        if (VarsAddressesResize(data, ChangeSizeRaise) != SUCCESS)
            return NO_MEMORY;
    }

    data->data[data->size++] = value;

    return SUCCESS;
}

int PopVarsAddresses(VarsAddresses *data, Address *value) {

    assert(data);

    if (VarsAddressesVerify(data) != SUCCESS)
        return ERROR;

    if (data->size - 1 < 0) {
        printf(RED "No addresses. Unable to pop value" END_OF_COLOR "\n");
        return NULL;
    }

    *value = data->data[--data->size];
    if (data->capacity > INITIAL_SIZE && data->capacity - data->size > data->capacity - data->capacity / INCREASE) {
        if (VarsAddressesResize(data, ChageSizeCut) != SUCCESS)
            return NO_MEMORY;
    }
    return SUCCESS;

}
static int VarsAddressesResize(VarsAddresses *data, VarsAddressesChangeSize action) {

    assert(data);

    if (action == ChangeSizeRaise) {
        data->capacity *= INCREASE;
    }
    else {
        data->capacity /= INCREASE;
    }

    data->data = (Address *) realloc (data->data, sizeof (Address) * data->capacity);
    if (!data->data) {
        printf(RED "vars addresses error: " END_OF_COLOR "memory allocation error\n");
        return NO_MEMORY;
    }

    memset(data->data + data->size, 0, data->capacity - data->size);

    return SUCCESS;
}

static int VarsAddressesVerify(VarsAddresses *data) {

    assert(data);

    if (!data->data) {
        printf(RED "vars addresses verifying: " END_OF_COLOR "NULL data\n");
        return ERROR;
    }
    if (data->size > data->capacity) {
        printf(RED "vars addresses verifying: " END_OF_COLOR "incorrect size or capacity\n");
        return ERROR;
    }

    return SUCCESS;
}