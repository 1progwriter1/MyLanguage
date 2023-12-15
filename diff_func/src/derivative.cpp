#include <stdio.h>
#include "derivative.h"
#include "calculate.h"
#include <assert.h>
#include "../MyLibraries/headers/systemdata.h"
#include "gen_graph_diff.h"
#include "../MyLibraries/headers/func.h"
#include <stdbool.h>
#include "bin_tree.h"
#include "diff_dsl.h"
#include <math.h>
#include "tree_output.h"

const char *SIMPLIFYING_PHRASES[] = {
    "Очевидно, что",
    "Ежу понятно, что",
    "Всем ясно, что",
    "Легко заметить, что",
    "Очевидно, что",
    "Надеюсь, вам очевидно, что",
    "Несложными преобразованиями получаем"
};

static TreeNode* TakeNodeDerivative(TreeStruct *tree, TreeNode *node, const size_t var_index);
static TreeNode* ConstEvaluate(ExpressionInfo *data, TreeNode *node, bool *is_changed);
static TreeNode* RemovingNeutralElements(ExpressionInfo *data, TreeNode *node, bool *is_chahged);
static TreeNode* AddSimplify(ExpressionInfo *data, TreeNode *node);
static TreeNode* SubSimplify(ExpressionInfo *data, TreeNode *node);
static TreeNode* MulSimplify(ExpressionInfo *data, TreeNode *node);
static TreeNode* DivSimplify(ExpressionInfo *data, TreeNode *node);
static TreeNode* PowSimplify(ExpressionInfo *data, TreeNode *node);
static int PrintSimplifying(TreeNode *fst, TreeNode *snd, Vector *vars, FILE *fn);

int TakeTreeDerivative(TreeStruct *tree, TreeStruct *diff_tree, const size_t var_index) {

    assert(tree);
    assert(diff_tree);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    diff_tree->root = TakeNodeDerivative(diff_tree, tree->root, var_index);
    if (!diff_tree->root) {
        printf(RED "Differentiation error" END_OF_COLOR "\n");
        return NULL_POINTER;
    }

    CountSubTreeSize(diff_tree->root);

    return SUCCESS;
}

static TreeNode* TakeNodeDerivative(TreeStruct *tree, TreeNode *node, const size_t var_index) {

    assert(tree);
    if (!node)
        return NULL;

    if (IsNum(node)) {
        return NEW(NUM(0), NULL, NULL);
    }
    if (IsVar(node)) {
        if (node->value.nvar == var_index)
            return NEW(NUM(1), NULL, NULL);
        return NEW(NUM(0), NULL, NULL);
    }

    #define DEF_OP(name, code, sym, args, diff, calc, ...)  \
        case (code): {                                      \
            return diff;                                    \
        }

    switch (node->value.operation) {
        #include "operations.h"
        default: {
            printf(RED "Missing operation" END_OF_COLOR "\n");
            return NULL;
        }
    }

    #undef DEF_OP

    return NULL;
}

int SimplifyTree(ExpressionInfo *data) {

    assert(data);
    assert(data->tree);

    if (TreeVerify(data->tree) != SUCCESS)
        return ERROR;

    bool is_changed = false;

    while (1) {

        is_changed = false;

        data->tree->root = ConstEvaluate(data, data->tree->root, &is_changed);
        if (!data->tree->root)
            return ERROR;

        data->tree->root = RemovingNeutralElements(data, data->tree->root, &is_changed);
        if (!data->tree->root)
            return ERROR;

        if (!is_changed)
            break;
    }

    if (TreeVerify(data->tree) != SUCCESS) {
        printf(RED "Incorrect tree after simplifying" END_OF_COLOR "\n");
        return ERROR;
    }

    CountSubTreeSize(data->tree->root);

    return SUCCESS;
}

static TreeNode* ConstEvaluate(ExpressionInfo *data, TreeNode *node, bool *is_changed) {

    assert(node);
    assert(data);
    assert(data->tree);
    assert(is_changed);

    TreeStruct *tree = data->tree;

    if (node->right) {
        if (!(IsNum(node->right))) {
            node->right = ConstEvaluate(data, node->right, is_changed);
            if (!node->right)
                return NULL;
        }
        if (node->left)
            if (!(IsNum(node->left))) {
                node->left = ConstEvaluate(data, node->left, is_changed);
                if (!node->left)
                    return NULL;
            }

        if ((!node->left && IsNum(node->right)) || (node->left && IsNum(node->left) && IsNum(node->right))) {
            bool error = 0;
            double ans = CalculateNode(node, NULL ,&error);
            if (error)
                return NULL;
            NodeDtor(tree, node);
            *is_changed = true;
            return NEW(NUM(ans), NULL, NULL);
        }
    }

    return node;
}

static TreeNode* RemovingNeutralElements(ExpressionInfo *data, TreeNode *node, bool *is_changed) {

    assert(data);
    assert(data->tree);
    assert(node);
    assert(is_changed);

    if (node->left)
        node->left = RemovingNeutralElements(data, node->left, is_changed);

    if (node->right)
        node->right = RemovingNeutralElements(data, node->right, is_changed);

    if (!node->left || !node->right)
        return node;

    if (!(IsOp(node)))
        return node;

    if (OperationVerify(node) != SUCCESS)
        return NULL;

    Operation operation = node->value.operation;

    if (operation == ADD)
        return AddSimplify(data, node);

    if (operation == SUB)
        return SubSimplify(data, node);

    if (operation == MUL)
        return MulSimplify(data, node);

    if (operation == DIV)
        return DivSimplify(data, node);

    if (operation == POW)
        return PowSimplify(data, node);

    return node;
}

static TreeNode* AddSimplify(ExpressionInfo *data, TreeNode *node) {

    assert(data);
    assert(data->tree);
    assert(node);

    TreeStruct *tree = data->tree;

    if (IsZero(node->left)) {

        TreeNode *ptr = NodeCopy(data->tree, node->right);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->left)) {

        TreeNode *ptr = NodeCopy(data->tree, node->left);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* SubSimplify(ExpressionInfo *data, TreeNode *node) {

    assert(data);
    assert(data->tree);
    assert(node);

    TreeStruct *tree = data->tree;

    if (IsNum(node->right) && IsZero(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* MulSimplify(ExpressionInfo *data, TreeNode *node) {

    assert(data);
    assert(data->tree);
    assert(node);

    TreeStruct *tree = data->tree;

    if (IsZero(node->left) || IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr) return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->left)) {

        TreeNode *ptr = NodeCopy(data->tree, node->right);
        if (!ptr) return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr) return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* DivSimplify(ExpressionInfo *data, TreeNode *node) {

    assert(data);
    assert(data->tree);
    assert(node);

    TreeStruct *tree = data->tree;

    if (IsZero(node->left)) {

        TreeNode *ptr = NEW(NUM(0), NULL, NULL);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

static TreeNode* PowSimplify(ExpressionInfo *data, TreeNode *node) {

    assert(data);
    assert(data->tree);
    assert(node);

    TreeStruct *tree = data->tree;

    if (IsZero(node->left) || IsOne(node->left)) {

        TreeNode *ptr = NEW(NUM(node->left->value.number), NULL, NULL);
        if (!ptr) return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsZero(node->right)) {

        TreeNode *ptr = NEW(NUM(1), NULL, NULL);
        if (!ptr) return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }
    if (IsOne(node->right)) {

        TreeNode *ptr = NodeCopy(tree, node->left);
        if (!ptr)  return NULL;

        PrintSimple

        NodeDtor(tree, node);
        return ptr;
    }

    return node;
}

int CountSubTreeSize(TreeNode *node) {

    assert(node);

    node->value.subtree_size = 1;

    if (node->value.nick != 0) {
        node->value.subtree_size = 1;
        return SUCCESS;
    }

    if (node->left) {
        CountSubTreeSize(node->left);
        node->value.subtree_size += node->left->value.subtree_size;
    }
    if (node->right) {
        CountSubTreeSize(node->right);
        node->value.subtree_size += node->right->value.subtree_size;
    }

    return SUCCESS;
}

static int PrintSimplifying(TreeNode *fst, TreeNode *snd, Vector *vars, FILE *fn) {

    assert(fst);
    assert(snd);
    assert(vars);
    assert(fn);

    fprintf(fn, "%s", SIMPLIFYING_PHRASES[randnum(0, sizeof (SIMPLIFYING_PHRASES) / sizeof (SIMPLIFYING_PHRASES[0]) - 1)]);

    fprintf(fn, "$$");
    if (PrintNodeLatex(fst, vars, fn) != SUCCESS)
        return ERROR;

    fprintf(fn, "\\Rightarrow ");

    if (PrintNodeLatex(snd, vars, fn) != SUCCESS)
        return ERROR;

    fprintf(fn, "$$\n");

    return SUCCESS;
}