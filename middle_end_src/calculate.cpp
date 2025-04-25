#include <stdio.h>
#include "calculate.h"
#include "systemdata.h"
#include "bin_tree.h"
#include <assert.h>
#include <math.h>
#include "func.h"
#include "diff_dsl.h"

enum NumOfAgrs {
    kOneArgsNum = 1,
    kTwoArgsNum = 2
};

int calculateTree(TreeStruct *tree, double *answer) {

    assert(tree);
    assert(answer);

    if (treeVerify(tree) != SUCCESS)
        return ERROR;

    bool error = 0;
    *answer = calculateNode(tree->root, &error);

    if (error) {
        printf(RED "Error when calculating the tree" END_OF_COLOR "\n");
        return ERROR;
    }

    return SUCCESS;
}

double calculateNode(TreeNode *node, bool *error) {

    assert(node);
    assert(error);

    if (*error) {
        return ERROR;
    }

    if (!((IsValType(node, UNARY_OP) && node->value.un_op <= LN) || (IsValType(node, BINARY_OP) && node->value.bin_op <= POW) || node->value.type == NUMBER)) {
        *error = true;
        return ERROR;
    }

    if (IsValType(node, NUMBER)) {
        return node->value.number;
    }

    if (operationVerify(node) != SUCCESS) {
        *error = true;
    }

    #define DEF_OP(name, code, sym, args, diff, calc, ...)  \
        case (code): {                                      \
            return calc;                                    \
        }

    int op_code = IsValType(node, UNARY_OP) ? (int) node->value.un_op : (int) node->value.bin_op;

    switch (op_code) {
        #include "operations.h"
        default: {
            printf(RED "Incorrect operation" END_OF_COLOR "\n");
            *error = true;
            return ERROR;
        }
    }

    #undef DEF_OP

    return SUCCESS;
}

int operationVerify(const TreeNode *node) {

    assert(node);

    if (IsValType(node, UNARY_OP)) {
            if (!node->right) {
                printf(RED "Missing argument. A null pointer was received." END_OF_COLOR "\n");
                return ERROR;
            }
            if (node->left) {
                printf(RED "An unnecessary argument for a operation that requires a single argument" END_OF_COLOR "\n");
                return ERROR;
            }
    }
    else if (IsValType(node, BINARY_OP)) {
        if (!node->left || !node->right) {
                printf(RED "Incorrect number of arguments for operation that requires two arguments" END_OF_COLOR "\n");
                return ERROR;
            }

    }
    else {
        printf(RED "Incorrect operation to calculate" END_OF_COLOR "\n");
        return ERROR;
    }


    if (IsBinaryOp(node, DIV)) {
        if (node->right->value.type == NUMBER && IsEqual(node->right->value.number, 0)) {
            printf(RED "Division by zero" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }
    if (IsUnaryOp(node, SQRT)) {
        if (node->right->value.type == NUMBER && node->value.number < 0) {
            printf(RED "The square root of a negative number" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }
    if (IsUnaryOp(node, LN)) {
        if (node->right->value.type == NUMBER && node->right->value.number <= 0) {
            printf(RED "The ln of a non-positive number" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }

    return SUCCESS;
}

int treeCopy(TreeStruct *tree_src, TreeStruct *tree_dst) {

    assert(tree_src);
    assert(tree_dst);

    if (treeRootCtor(tree_dst) != SUCCESS)
        return ERROR;

    nodeDtor(tree_dst, tree_dst->root);
    tree_dst->root = nodeCopy(tree_dst, tree_src->root);
    if (!tree_dst->root)
        return ERROR;

    return SUCCESS;
}

TreeNode* nodeCopy(TreeStruct *tree, TreeNode *src) {

    assert(tree);
    assert(src);

    TreeNode *ptr = treeNodeNew(tree, src->value,  NULL, NULL);
    if (!ptr)
        return NULL;

    if (src->left) {
        ptr->left = nodeCopy(tree, src->left);
        if (!ptr->left)
            return NULL;
    }

    if (src->right) {
        ptr->right = nodeCopy(tree, src->right);
        if (!ptr->right) {
            nodeDtor(tree, ptr->left);
            return NULL;
        }
    }

    return ptr;
}
