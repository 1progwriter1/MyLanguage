#ifndef DIFFERENTIATOR
#define DIFFERENTIATOR

#include "bin_tree.h"

// int CalculateTree(TreeStruct *tree, Vector *vars, double *answer);

// double GetVarialble();

int TreeCopy(TreeStruct *tree_src, TreeStruct *tree_dst);

TreeNode *NodeCopy(TreeStruct *tree, TreeNode *src);

// double CalculateNode(TreeNode *node, Vector *vars, bool *error);

// int OperationVerify(const TreeNode *node);

// int GetNumOfArgs(const Operation operation);

#endif