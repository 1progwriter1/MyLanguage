#ifndef GENERATE_OPERATIONS
#define GENERATE_OPERATIONS

#include "backend_data.h"
#include "gen_asm_x86.h"
#include "variables_x86.h"

int genAdd(TreeNode *node, CodeGenData *data, ValueSrc *src);

int genSub(TreeNode *node, CodeGenData *data, ValueSrc *src);

int genOutput(TreeNode *node, CodeGenData *data);

int moveToRegister(CodeGenData *data, ValueSrc *src);

int genMul(TreeNode *node, CodeGenData *data, ValueSrc *src);

#endif