#ifndef DIFFERENTIATOR
#define DIFFERENTIATOR

#include "../lib_src/bin_tree.h"

int calculateTree(struct TreeStruct *tree, double *answer);

int treeCopy(TreeStruct *tree_src, TreeStruct *tree_dst);

TreeNode *nodeCopy(TreeStruct *tree, TreeNode *src);

double calculateNode(TreeNode *node, bool *error);

int operationVerify(const TreeNode *node);

#endif