#ifndef DERIVATIVE_DIFF
#define DERIVATIVE_DIFF

#include "bin_tree.h"

struct ExpressionInfo {
    TreeStruct *tree;
    Vector *vars;
    FILE *fn;
};

int TakeTreeDerivative(TreeStruct *tree, TreeStruct *diff_tree, const size_t var_index);

int SimplifyTree(ExpressionInfo *data);

int CountSubTreeSize(TreeNode *node);

#endif