#ifndef FILE_READ_DIFF
#define FILE_READ_DIFF

#include "../lib_src/bin_tree.h"
#include "../../MyLibraries/headers/my_vector.h"

#define BACK_SRC_FILE "prog.txt"

struct FileBuffer {
    char *buf;
    size_t index;
    Vector *names_table;
};

int readFileLang(TreeStruct *tree, const char *filename, Vector *names_table);


#endif