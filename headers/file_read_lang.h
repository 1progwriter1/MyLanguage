#ifndef FILE_READ_DIFF
#define FILE_READ_DIFF

#include "../diff_func/headers/bin_tree.h"

#define PROG_FILE "prog.txt"

struct FileBuffer {
    char *buf;
    size_t index;
};

int ReadFileLang(TreeStruct *tree, const char *filename);


#endif