#include <stdio.h>
#include "file_func.h"
#include <assert.h>
#include "systemdata.h"
#include "func.h"
#include <sys/stat.h>
#include <stdlib.h>

char *readFileToStruct(struct Text *file_data) {

    assert(file_data);
    assert(file_data->filename);

    char *buf = (char *) calloc (file_data->file_len + 1, sizeof (char));
    fread(buf, sizeof (char), file_data->file_len, file_data->filename);

    file_data->NumOfStr = numOfStr(buf, file_data->file_len);

    return buf;
}

char *readFileToBuffer(const char *filename) {

    assert(filename);

    unsigned long len_of_file = lenOfFile(filename);
    FILE *fn = openFile(filename, READ);
    if (!fn)
        return NULL;

    char *buf = (char *) calloc (len_of_file + 1, sizeof (char));
    fread(buf, sizeof (char), len_of_file, fn);
    if (ferror(fn)) {
        printf(RED "Ошибка при чтении файла" END_OF_COLOR "\n");
        return NULL;
    }

    closeFile(fn);

    return buf;
}

long long int numOfStr(const char *str, size_t len) {

    assert(str);

    int c = 0, col = 1;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\n')
            col++;
    }
    return col;
}

void setStructText(struct Text *data, const char *filename) {

    assert(data);

    data->file_len   = lenOfFile(filename);
    data->filename   = openFile (filename, READ);
    data->NumOfWords = 0;
    data->NumOfStr   = 0;

}

void cleanFile(const char *filename) {

    assert(filename);

    FILE *fn = openFile(filename, WRITE);
}

FILE *openFile(const char *filename, const char mode[]) {

    assert(filename);

    FILE *fn = fopen(filename, mode);

    if (fn == nullptr)
        perror("");

    return fn;
}

void closeFile(FILE *fn) {

    assert(fn);

    if (fn != stdout)
        if (fclose(fn) != 0)
            fprintf(stderr, "File not closed\n");
}

unsigned long lenOfFile(const char *filename) {

    assert(filename);

    struct stat buf = {};
    if (stat(filename, &buf) == - 1)
        return ERROR;

    return buf.st_size;
}
