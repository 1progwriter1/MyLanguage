#ifndef FILE_FUNCTIONS
#define FILE_FUNCTIONS

#include <stdio.h>

struct Text {
        long long int file_len;
        FILE *filename;
        long long int NumOfWords;
        long long int NumOfStr;
};

void setStructText(struct Text *data, const char *filename);

void cleanFile(const char *filename);

FILE *openFile(const char *filename, const char mode[]);

void closeFile(FILE *fn);

unsigned long lenOfFile(const char *filename);

long long int numOfStr(const char *str, size_t len);

char *readFileToStruct(struct Text *file_data);

char *readFileToBuffer(const char *filename);

#endif
