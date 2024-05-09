#ifndef ASM_CODE_GEN
#define ASM_CODE_GEN

#include "../bin_tree/bin_tree.h"

enum VarPlace {
    VarPlaceRAM,
    VarPlaceREGS,
};

struct Address {
    size_t var_code;
    VarPlace place;
    size_t var_index;
};

struct GenIndexes {
    size_t cur_if;
    size_t cur_while;
    size_t cur_ram_ind;
    size_t cur_reg_ind;
    size_t cur_func_exe;
};

struct Variables {
    Vector *variables;
    Vector *names_table;
};

struct CodeGenData {
    FILE *fn;
    Variables vars;
    GenIndexes indexes;
};

int genAsmCode(TreeStruct *tree, Vector *names_table, const char *filename);

bool isPunct(TreeNode *node, Punctuation sym);
bool isBinOp(TreeNode *node, Binary_Op operation);
bool isUnOp(TreeNode *node, Unary_Op operation);
bool isKeyOp(TreeNode *node, Key_Op operation);
bool isType(TreeNode *node, ValueType type);

#endif