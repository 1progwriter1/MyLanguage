#include <stdio.h>
#include "bin_tree.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/func.h"
#include <string.h>
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/calculate.h"

int treeRootCtor(TreeStruct *tree) {

    assert(tree);

    tree->root = treeNodeNew(tree, (Token) {}, NULL, NULL);
    if (!tree->root)
        return NO_MEMORY;

    return SUCCESS;
}

TreeNode *treeNodeNew(TreeStruct *tree, Token value, TreeNode *left, TreeNode *right) {

    TreeNode *node = (TreeNode *) calloc (1, sizeof (TreeNode));
    if (!node)
        return NULL;

    node->value = value;
    node->left  = left;
    node->right = right;

    tree->size++;

    return node;
}

TreeNode *treeNodeNewSafe(TreeStruct *tree, Token value, TreeNode *left, TreeNode *right) {

    assert(tree);

    if (value.type == UNARY_OP)
        if (!right)
            return NULL;

    if (value.type == BINARY_OP || value.type == KEY_OP)
        if (!left || !right)
            return NULL;

    TreeNode *node = treeNodeNew(tree, value, left, right);
    if (!node) return NULL;

    return node;
}

int treeRootDtor(TreeStruct *tree) {

    assert(tree);

    if (!tree->root)
        return SUCCESS;

    if (tree->root->left || tree->root->right) {
        nodeDtor(tree, tree->root);
    }

    tree->root = NULL;
    tree->size = 0;

    return SUCCESS;
}

int nodeDtor(TreeStruct *tree, TreeNode *node) {

    assert(tree);

    if (!node)
        return SUCCESS;

    if (node->left) {
        nodeDtor(tree, node->left);
        node->left = NULL;
    }
    if (node->right) {
        nodeDtor(tree, node->right);
        node->right = NULL;
    }

    if (node->value.type == STRING) {
        free(node->value.string);
        node->value.string = NULL;
    }
    free(node);

    tree->size--;

    return SUCCESS;
}

int treeVerify(TreeStruct *tree) {

    assert(tree);

    size_t col_nodes = 0;
    if (tree->root)
        col_nodes = 1;
    else {
        printf(RED "tree error: " END_OF_COLOR "NULL root\n");
        return NULL_POINTER;
    }

    walkTree(tree->root, &col_nodes);

    if (col_nodes != tree->size) {
        printf(RED "tree error: " END_OF_COLOR "incorrect size of the tree\n");
        return ERROR;
    }

    return SUCCESS;
}

int walkTree(TreeNode *node, size_t *col_nodes) {

    assert(node);
    assert(col_nodes);

    if (node->left) {
        *col_nodes += 1;
        walkTree(node->left, col_nodes);
    }
    if (node->right) {
        *col_nodes += 1;
        walkTree(node->right, col_nodes);
    }

    return SUCCESS;
}