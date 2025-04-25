#include <stdio.h>
#include "derivative.h"
#include "calculate.h"
#include <assert.h>
#include "systemdata.h"
#include "func.h"
#include <stdbool.h>
#include "bin_tree.h"
#include "diff_dsl.h"
#include <math.h>

static TreeNode* constEvaluate(TreeStruct *tree, TreeNode *node, bool *is_changed);
static TreeNode* removingNeutralElements(TreeStruct *tree, TreeNode *node, bool *is_changed);
static TreeNode* addSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* subSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* mulSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* divSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* powSimplify(TreeStruct *tree, TreeNode *node);

int simplifyTree(TreeStruct *tree) {

    assert(tree);

    if (treeVerify(tree) != SUCCESS)
        return ERROR;

    bool is_changed = false;

    while (1) {

        is_changed = false;

        tree->root = constEvaluate(tree, tree->root, &is_changed);
        if (!tree->root)
            return ERROR;

        tree->root = removingNeutralElements(tree, tree->root, &is_changed);
        if (!tree->root)
            return ERROR;

        if (!is_changed)
            break;
    }

    if (treeVerify(tree) != SUCCESS) {
        printf(RED "Incorrect tree after simplifying" END_OF_COLOR "\n");
        return ERROR;
    }

    return SUCCESS;
}

static TreeNode* constEvaluate(TreeStruct *tree, TreeNode *node, bool *is_changed) {

    assert(node);
    assert(tree);
    assert(is_changed);

    if (node->right)
        node->right = constEvaluate(tree, node->right, is_changed);

    if (node->left)
        node->left = constEvaluate(tree, node->left, is_changed);

    if (!node->left && !node->right)
        return node;

    if ((!node->left && IsValType(node->right, NUMBER)) || (node->left && IsValType(node->left, NUMBER) && IsValType(node->right, NUMBER))) {
        bool error = 0;
        double ans = calculateNode(node ,&error);
        if (error)
            return node;
        nodeDtor(tree, node);
        *is_changed = true;
        return NEW(NUM(ans), NULL, NULL);
    }

    return node;
}

static TreeNode* removingNeutralElements(TreeStruct *tree, TreeNode *node, bool *is_changed) {

    assert(tree);
    assert(node);
    assert(is_changed);

    if (node->left)
        node->left = removingNeutralElements(tree, node->left, is_changed);

    if (node->right)
        node->right = removingNeutralElements(tree, node->right, is_changed);

    if (!node->left || !node->right)
        return node;

    if (!(IsValType(node, BINARY_OP)))
        return node;

    if (operationVerify(node) != SUCCESS)
        return NULL;

    Binary_Op operation = node->value.bin_op;

    if (operation == ADD)
        return addSimplify(tree, node);

    if (operation == SUB)
        return subSimplify(tree, node);

    if (operation == MUL)
        return mulSimplify(tree, node);
    if (operation == DIV)
        return divSimplify(tree, node);

    if (operation == POW)
        return powSimplify(tree, node);

    return node;
}

static TreeNode* addSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left)) {

        TreeNode *ptr = nodeCopy(tree, node->right);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->left)) {

        TreeNode *ptr = nodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* subSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsValType(node->right, NUMBER) && IsZero(node->right)) {

        TreeNode *ptr = nodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* mulSimplify(TreeStruct *tree, TreeNode *node) {
    assert(tree);
    assert(node);

    if (IsZero(node->left) || IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr) return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->left)) {

        TreeNode *ptr = nodeCopy(tree, node->right);
        if (!ptr) return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->right)) {

        TreeNode *ptr = nodeCopy(tree, node->left);
        if (!ptr) return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* divSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = nodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* powSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left) || IsOne(node->left)) {

        TreeNode *ptr = NEW(NUM(node->left->value.number), NULL, NULL);
        if (!ptr) return NULL;

        nodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(1), NULL, NULL);
        if (!ptr) return NULL;

        nodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = nodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        nodeDtor(tree, node);
        return ptr;
    }

    return node;
}
