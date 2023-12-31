#ifndef DIFFERENTIATOR
#define DIFFERENTIATOR

#include "../bin_tree/bin_tree.h"

int CalculateTree(TreeStruct *tree, double *answer);

int TreeCopy(TreeStruct *tree_src, TreeStruct *tree_dst);

TreeNode *NodeCopy(TreeStruct *tree, TreeNode *src);

double CalculateNode(TreeNode *node, bool *error);

int OperationVerify(const TreeNode *node);

#endif