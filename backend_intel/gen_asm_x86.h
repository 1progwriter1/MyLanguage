#ifndef ASM_CODE_GEN
#define ASM_CODE_GEN

#include "../lib_src/bin_tree.h"
#include "variables_x86.h"
#include "backend_data.h"
#include "gen_operations.h"

#define CODE_GEN_ASSERT assert(node);                   \
                        assert(data);                   \
                        assert(data->fn);

int genAsmCode(TreeStruct *tree, Vector *names_table, const char *filename);

bool isPunct(TreeNode *node, Punctuation sym);
bool isBinOp(TreeNode *node, Binary_Op operation);
bool isUnOp(TreeNode *node, Unary_Op operation);
bool isKeyOp(TreeNode *node, Key_Op operation);
bool isType(TreeNode *node, ValueType type);

int genFunction(TreeNode *node, CodeGenData *data);
int genNewLine(TreeNode *node, CodeGenData *data);
int genIf(TreeNode *node, CodeGenData *data);
int genWhile(TreeNode *node, CodeGenData *data);
int genAssign(TreeNode *node, CodeGenData *data);
int genExpression(TreeNode *node, CodeGenData *data, ValueSrc *src);
int genLogicalJump(TreeNode *node, CodeGenData *data);
int genUnaryOp(TreeNode *node, CodeGenData *data);
int genBinaryOp(TreeNode *node, CodeGenData *data, ValueSrc *src);
int genInput(TreeNode *node, CodeGenData *data);
int genCall(TreeNode *node, CodeGenData *data);
int genRet(TreeNode *node, CodeGenData *data);

void createStart(FILE *fn);
void createRoData(CodeGenData *data);
void printStr(CodeGenData *data, char *str);

#endif