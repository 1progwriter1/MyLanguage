#ifndef VARIABLES_PROCESSING
#define VARIABLES_PROCESSING

#include "../../MyLibraries/headers/my_vector.h"
#include "gen_asm.h"

int prepareData(struct CodeGenData *data, const char *filename, Vector *names_table);

void dtorData(CodeGenData *data);

int createSegment(CodeGenData *data, TreeNode *node);
int destroySegment(CodeGenData *data);

int writeVariable(CodeGenData *data, TreeNode *node);

size_t getVarIndex(CodeGenData *data, size_t index);
size_t getVarPlace(CodeGenData *data, size_t index);

int getVariableValue(CodeGenData *data, size_t var_code);

int saveArgs(CodeGenData *data, TreeNode *node);
int restoreArgs(CodeGenData *data, TreeNode *node);

void zeroRegs(CodeGenData *data);
void setSegment(CodeGenData *data);

void incRdx(CodeGenData *data);
void setRbx(CodeGenData *data, size_t index);

void saveRdxRcx(CodeGenData *data);
void restoreRdxRcx(CodeGenData *data);

#endif