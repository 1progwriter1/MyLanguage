#ifndef X86_BACKEND_DATA
#define X86_BACKEND_DATA

#include "../../MyLibraries/headers/my_vector.h"

#include <stdio.h>

const size_t VALUE_SIZE = sizeof(long long);
extern const char *REGS_NAMES[];

const size_t NUMBER_OF_USED = 13;

enum Registers {
    RAX = 0,    RBX = 1,    RCX = 2,    RDX = 3,
    RSI = 4,    RDI = 5,    RBP = 6,    RSP = 7,
    R8  = 8,    R9  = 9,    R10 = 10,   R11 = 11,
    R12 = 12,   R13 = 13,   R14 = 14,   R15 = 15,
};

extern const Registers ARGUMENTS_SRC[];

struct Address {
    size_t var_code;
    size_t var_index;
};

struct UsedRegisters {
    Registers reg;
    bool is_used;
};

struct GenIndexes {
    size_t cur_if;
    size_t cur_while;
    size_t cur_stack_ind;
    size_t cur_func_exe;
};

struct Variables {
    Vector *variables;
    Vector *names_table;
    UsedRegisters used_regs[NUMBER_OF_USED];
};

struct CodeGenData {
    FILE *fn;
    long int offset;
    Variables vars;
    Vector *str_data;
    GenIndexes indexes;
};

enum ValueSrcType {
    TypeNumber = 0,
    TypeStack  = 1,
    TypeReg    = 2,
};

struct ValueSrc {
    ValueSrcType type;
    union {
        size_t index;
        Registers reg;
        long long number;
    };
};



#endif