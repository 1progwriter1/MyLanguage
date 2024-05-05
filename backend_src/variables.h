#ifndef VARIABLES_PROCESSING
#define VARIABLES_PROCESSING

#include "../../MyLibraries/headers/my_vector.h"
#include "gen_asm.h"

int prepareData(struct CodeGenData *data, const char *filename, Vector *names_table);

void dtorData(CodeGenData *data);

int createSegment(CodeGenData *data, TreeNode *node);
int destroySegment(CodeGenData *data);

#endif