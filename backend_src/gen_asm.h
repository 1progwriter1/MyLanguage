#ifndef ASM_CODE_GEN
#define ASM_CODE_GEN

#include "../bin_tree/bin_tree.h"

#define DST_FILE "asm.txt"

enum VarPlace {
    VarPlaceRAM,
    VarPlaceREGS,
};

struct Address {
    size_t var_code;
    VarPlace place;
    size_t var_index;
};

struct CodeGenData {
    FILE *fn;
    size_t cur_func;
    size_t cur_if;
    size_t cur_while;
    Vector *vars_data;
    Vector *local_vars;
    size_t cur_ram_ind;
    size_t cur_reg_ind;
    size_t cur_func_exe;
};

int genAsmCode(TreeStruct *tree, const char *filename);

#endif

/*
call recursion
pop rax
pop rbx
*/