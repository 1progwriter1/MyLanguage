#include <stdio.h>
#include "../headers/derivative.h"
#include "../headers/calculate.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/func.h"
#include <stdbool.h>
#include "../bin_tree/bin_tree.h"
#include "../headers/diff_dsl.h"
#include <math.h>

static TreeNode* ConstEvaluate(TreeStruct *tree, TreeNode *node, bool *is_changed);
static TreeNode* RemovingNeutralElements(TreeStruct *tree, TreeNode *node, bool *is_chahged);
static TreeNode* AddSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* SubSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* MulSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* DivSimplify(TreeStruct *tree, TreeNode *node);
static TreeNode* PowSimplify(TreeStruct *tree, TreeNode *node);

int SimplifyTree(TreeStruct *tree) {

    assert(tree);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    bool is_changed = false;

    while (1) {

        is_changed = false;

        tree->root = ConstEvaluate(tree, tree->root, &is_changed);
        if (!tree->root)
            return ERROR;

        tree->root = RemovingNeutralElements(tree, tree->root, &is_changed);
        if (!tree->root)
            return ERROR;

        if (!is_changed)
            break;
    }

    if (TreeVerify(tree) != SUCCESS) {
        printf(RED "Incorrect tree after simplifying" END_OF_COLOR "\n");
        return ERROR;
    }

    return SUCCESS;
}

static TreeNode* ConstEvaluate(TreeStruct *tree, TreeNode *node, bool *is_changed) {

    assert(node);
    assert(tree);
    assert(is_changed);

    if (node->right)
        node->right = ConstEvaluate(tree, node->right, is_changed);

    if (node->left)
        node->left = ConstEvaluate(tree, node->left, is_changed);

    if (!node->left && !node->right)
        return node;

    if ((!node->left && IsValType(node->right, NUMBER)) || (node->left && IsValType(node->left, NUMBER) && IsValType(node->right, NUMBER))) {
        bool error = 0;
        double ans = CalculateNode(node ,&error);
        if (error)
            return node;
        NodeDtor(tree, node);
        *is_changed = true;
        return NEW(NUM(ans), NULL, NULL);
    }

    return node;
}

static TreeNode* RemovingNeutralElements(TreeStruct *tree, TreeNode *node, bool *is_changed) {

    assert(tree);
    assert(node);
    assert(is_changed);

    if (node->left)
        node->left = RemovingNeutralElements(tree, node->left, is_changed);

    if (node->right)
        node->right = RemovingNeutralElements(tree, node->right, is_changed);

    if (!node->left || !node->right)
        return node;

    if (!(IsValType(node, BINARY_OP)))
        return node;

    if (OperationVerify(node) != SUCCESS)
        return NULL;

    Binary_Op operation = node->value.bin_op;

    if (operation == ADD)
        return AddSimplify(tree, node);

    if (operation == SUB)
        return SubSimplify(tree, node);

    if (operation == MUL)
        return MulSimplify(tree, node);

    if (operation == DIV)
        return DivSimplify(tree, node);

    if (operation == POW)
        return PowSimplify(tree, node);

    return node;
}

static TreeNode* AddSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left)) {

        TreeNode *ptr = NodeCopy(tree, node->right);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->left)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* SubSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsValType(node->right, NUMBER) && IsZero(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* MulSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left) || IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr) return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->left)) {

        TreeNode *ptr = NodeCopy(tree, node->right);
        if (!ptr) return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr) return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* DivSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* PowSimplify(TreeStruct *tree, TreeNode *node) {

    assert(tree);
    assert(node);

    if (IsZero(node->left) || IsOne(node->left)) {

        TreeNode *ptr = NEW(NUM(node->left->value.number), NULL, NULL);
        if (!ptr) return NULL;

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(1), NULL, NULL);
        if (!ptr) return NULL;

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}
