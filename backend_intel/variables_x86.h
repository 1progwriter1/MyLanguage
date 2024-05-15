#ifndef VARIABLES_PROCESSING
#define VARIABLES_PROCESSING

#include "../../MyLibraries/headers/my_vector.h"
#include "gen_asm_x86.h"

enum ValueType {
    TypeNumber = 0,
    TypeStack  = 1,
    TypeReg    = 2,
};

enum Registers {
    RAX,    RBX,    RCX,    RDX,
    RSI,    RDI,    RBP,    RSP,
    R8,     R9,     R10,    R11,
    R12,    R13,    R14,    R15
};

struct ValueSrc {
    ValueType type;
    union {
        size_t index;
        Registers reg;
        long long number;
    };
};

int prepareData(struct CodeGenData *data, const char *filename, Vector *names_table);

void dtorData(CodeGenData *data);

int createSegment(CodeGenData *data, TreeNode *node);
int destroySegment(CodeGenData *data);

int writeVariable(CodeGenData *data, TreeNode *node, ValueSrc src);

size_t getVarIndex(CodeGenData *data, size_t index);
size_t getVarPlace(CodeGenData *data, size_t index);

int getVariableValue(CodeGenData *data, size_t var_code);

void setRbp(CodeGenData *data);

void incRdx(CodeGenData *data);
void setRbx(CodeGenData *data, size_t index);

void saveRdxRcx(CodeGenData *data);
void restoreRdxRcx(CodeGenData *data);

#endif