#ifndef FILE_READ_DIFF
#define FILE_READ_DIFF

#include "../bin_tree/bin_tree.h"

#define SRC_FILE "prog.txt"

struct FileBuffer {
    char *buf;
    size_t index;
};

int ReadFileLang(TreeStruct *tree, const char *filename);


#endif