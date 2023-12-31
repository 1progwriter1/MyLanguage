#ifndef BIN_TREE_FUNC
#define BIN_TREE_FUNC

#include <stdio.h>
#include "../headers/my_vector.h"

typedef Token Tree_t;
#define tree_output_id "%d"

struct TreeNode {
    Tree_t value;
    TreeNode *left;
    TreeNode *right;
};

struct TreeStruct {
    TreeNode *root;
    size_t size;
};

int TreeRootCtor(TreeStruct *tree);

TreeNode *TreeNodeNew(TreeStruct *tree, Token value, TreeNode *left, TreeNode* rigth);

TreeNode *TreeNodeNewSafe(TreeStruct *tree, Token value, TreeNode *left, TreeNode *right);

int TreeRootDtor(TreeStruct *tree);

int NodeDtor(TreeStruct *tree, TreeNode *node);

int TreeVerify(TreeStruct *tree);

#endif