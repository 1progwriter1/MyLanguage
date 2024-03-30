#ifndef ASM_CODE_GEN
#define ASM_CODE_GEN

#include "../bin_tree/bin_tree.h"
#include "vars_addresses.h"
#include "../Stack/stack.h"

#define DST_FILE "asm.txt"

struct CodeGenData {
    FILE *fn;
    int cur_func;
    size_t cur_if;
    size_t cur_while;
    VarsAddresses vars_data;
    Stack local_vars;
    size_t cur_ram_ind;
    size_t cur_reg_ind;
    int cur_func_exe;
};

int GenAsmCode(TreeStruct *tree, const char *filename);

#endif

/*
call recursion
pop rax
pop rbx
*/