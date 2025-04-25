#ifndef VARIABLES_PROCESSING
#define VARIABLES_PROCESSING

#include "my_vector.h"
#include "gen_asm_x86.h"
#include "backend_data.h"

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

void setRegisters(CodeGenData *data);
bool findVariable(CodeGenData *data, size_t *index, size_t code);

void saveRdxRcx(CodeGenData *data);
void restoreRdxRcx(CodeGenData *data);

int findFreeRegister(CodeGenData *data);
void printPlace(CodeGenData *data, ValueSrc src);

#endif
