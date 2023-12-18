#ifndef ASM_CODE_GEN
#define ASM_CODE_GEN

#include "../diff_func/headers/bin_tree.h"

#define ASM_FILE "asm.txt"

struct CodeGenData {
    FILE *fn;
    size_t cur_var;
    size_t cur_func;
};

int GenAsmCode(TreeStruct *tree, const char *filename);

#endif