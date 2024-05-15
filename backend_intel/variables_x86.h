#ifndef VARIABLES_PROCESSING
#define VARIABLES_PROCESSING

#include "../../MyLibraries/headers/my_vector.h"
#include "gen_asm_x86.h"

enum ValueSrcType {
    TypeNumber = 0,
    TypeStack  = 1,
    TypeReg    = 2,
};

enum Registers {
    RAX = 0,    RAB = 1,    RCX = 2,    RDX = 3,
    RSI = 4,    RDI = 5,    RBP = 6,    RSP = 7,
    R8  = 8,    R9  = 9,    R10 = 10,   R11 = 11,
    R12 = 12,   R13 = 13,   R14 = 14,   R15 = 15,
};

struct ValueSrc {
    ValueSrcType type;
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