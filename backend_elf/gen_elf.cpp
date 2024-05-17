#include <stdio.h>
#include "gen_elf.h"
#include "../../MyLibraries/headers/systemdata.h"
#include <assert.h>
#include "../../MyLibraries/headers/file_func.h"
#include <stdlib.h>

int genElf(TreeStruct *tree, const char *output_file) {

    assert(tree);
    assert(output_file);

    Vector code = {};
    CodeGenData data = {&code};

    if (prepareToGen(&data) != SUCCESS) {
        endGen(&data);
        return ERROR;
    }

    if (createHeader(&data) != SUCCESS) {
        endGen(&data);
        return ERROR;
    }

    if (writeInFile(&data, output_file) != SUCCESS) {
        endGen(&data);
        return ERROR;
    }

    return SUCCESS;
}

int prepareToGen(CodeGenData *data) {

    assert(data);

    data->tmp = (char *) calloc (1, sizeof(char));
    if (!data->tmp) return ERROR;

    if (vectorCtor(data->code, 8, sizeof(char)) != SUCCESS) {
        free(data->tmp);
        return ERROR;
    }

    return SUCCESS;
}

void endGen(CodeGenData *data) {

    assert(data);

    vectorDtor(data->code);
    free(data->tmp);
}

int writeInFile(CodeGenData *data, const char *filename) {

    assert(data);
    assert(filename);

    FILE *fn = openFile(filename, WRITE);
    if (!fn)    return FILE_OPEN_ERROR;

    size_t elements_written = fwrite(data->code->data, sizeof(char), data->code->size, fn);
    if (elements_written != data->code->size) {
        closeFile(fn);
        printf(RED "elf gen error: " END_OF_COLOR "failed to write in file\n");
        return ERROR;
    }

    closeFile(fn);

    return SUCCESS;
}

int createHeader(CodeGenData *data) {

    assert(data);

    char header[] = {0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    for (size_t i = 0; i < sizeof(header); i++) {
        *data->tmp = header[i];
        if (pushBack(data->code, data->tmp) != SUCCESS) {
            printf(RED "error: " END_OF_COLOR "header creation failed\n");
            return ERROR;
        }
    }

    return SUCCESS;
}