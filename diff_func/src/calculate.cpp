#include <stdio.h>
#include "../headers/calculate.h"
#include "../../../MyLibraries/headers/systemdata.h"
#include "../headers/bin_tree.h"
#include <assert.h>
#include <math.h>
#include "../../../MyLibraries/headers/func.h"
#include "../headers/diff_dsl.h"

enum NumOfAgrs {
    kOneArgsNum = 1,
    kTwoArgsNum = 2
};

/* static int GetVariables(Vector *vars);

int CalculateTree(TreeStruct *tree, Vector *vars, double *answer) {

    assert(tree);
    assert(answer);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    if (vars) {
        if (VarsVerify(vars) != SUCCESS)
            return ERROR;

        if (vars->size > 0) {
            GetVariables(vars);
        }
    }

    bool error = 0;
    *answer = CalculateNode(tree->root, vars, &error);

    if (error) {
        printf(RED "Error when calculating the tree" END_OF_COLOR "\n");
        return ERROR;
    }

    return SUCCESS;
}

double CalculateNode(TreeNode *node, Vector *vars, bool *error) {

    assert(node);
    assert(error);

    if (*error) {
        return ERROR;
    }

    if (node->value.type == NUMBER) {
        return node->value.number;
    }

    if (node->value.type == VARIABLE) {
        if (!vars) {
            printf(RED "Missing values of variables" END_OF_COLOR "\n");
            *error = true;
            return ERROR;
        }
        return vars->data[node->value.nvar].value;
    }

    if (OperationVerify(node) != SUCCESS) {
        *error = true;
    }

    #define DEF_OP(name, code, sym, args, diff, calc, ...)  \
        case (code): {                                      \
            return calc;                                    \
        }

    switch (node->value.operation) {
        #include "../headers/operations.h"
        default: {
            printf(RED "Incorrect operation" END_OF_COLOR "\n");
            *error = true;
            return ERROR;
        }
    }

    #undef DEF_OP

    return SUCCESS;
} */

/* int OperationVerify(const TreeNode *node) {

    assert(node);

    switch (GetNumOfArgs(node->value.operation)) {
        case (kOneArgsNum): {
            if (!node->right) {
                printf(RED "Missing argument. A null pointer was received." END_OF_COLOR "\n");
                return ERROR;
            }
            if (node->left) {
                printf(RED "An unnecessary argument for a operation that requires a single argument" END_OF_COLOR "\n");
                return ERROR;
            }
            break;
        }
        case (kTwoArgsNum): {
            if (!node->left || !node->right) {
                printf(RED "Incorrect number of arguments for operation that requires two arguments" END_OF_COLOR "\n");
                return ERROR;
            }
            break;
        }
        default: {
            printf(RED "Invalid number of arguments" END_OF_COLOR "\n");
            return ERROR;
        }
    }
    Operation operation = node->value.operation;
    if (operation == DIV) {
        if (node->right->value.type == NUMBER && IsEqual(node->right->value.number, 0)) {
            printf(RED "Division by zero" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }
    if (operation == SQRT) {
        if (node->right->value.type == NUMBER && node->value.number < 0) {
            printf(RED "The square root of a negative number" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }
    if (operation == LN) {
        if (node->right->value.type == NUMBER && node->right->value.number <= 0) {
            printf(RED "The ln of a non-positive number" END_OF_COLOR "\n");
            return ERROR;
        }
        return SUCCESS;
    }

    return SUCCESS;
} */

/* int GetNumOfArgs(const Operation operation) {

    #define DEF_OP(name, code, sym, args, ...)  \
        case (code): {                          \
            return args;                        \
        }


    switch (operation) {

        #include "../headers/operations.h"

        default: {
            return 0;
        }
    }

    return 0;
} */

int TreeCopy(TreeStruct *tree_src, TreeStruct *tree_dst) {

    assert(tree_src);
    assert(tree_dst);

    if (TreeRootCtor(tree_dst) != SUCCESS)
        return ERROR;

    NodeDtor(tree_dst, tree_dst->root);
    tree_dst->root = NodeCopy(tree_dst, tree_src->root);
    if (!tree_dst->root)
        return ERROR;

    return SUCCESS;
}

TreeNode* NodeCopy(TreeStruct *tree, TreeNode *src) {

    assert(tree);
    assert(src);

    TreeNode *ptr = TreeNodeNew(tree, src->value,  NULL, NULL);
    if (!ptr)
        return NULL;

    if (src->left) {
        ptr->left = NodeCopy(tree, src->left);
        if (!ptr->left)
            return NULL;
    }

    if (src->right) {
        ptr->right = NodeCopy(tree, src->right);
        if (!ptr->right) {
            NodeDtor(tree, ptr->left);
            return NULL;
        }
    }

    return ptr;
}

/* static int GetVariables(Vector *vars) {

    assert(vars);

    for (size_t i = 0; i < vars->size; i++) {
        printf(MAGENTA "Enter %c: " END_OF_COLOR "\n", vars->data[i].name);
        vars->data[i].value = GetVarialble();
    }

    return SUCCESS;
}

double GetVarialble() {

    double var = 0;

    bool correct = 1;
    do {
        correct = 1;
        if (scanf("%lg", &var) != 1) {
            printf(RED "Incorrect input. Try again: " END_OF_COLOR);
            buf_clear();
            correct = 0;
        }
    } while (!correct);

    return var;
} */