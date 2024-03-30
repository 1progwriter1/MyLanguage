#include <stdio.h>
#include "../headers/gen_prog.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/diff_dsl.h"
#include "../headers/key_words_codes.h"

static int TranslateFunction(TreeNode *node, FILE *fn);
static int TranslateBody(TreeNode *node, FILE *fn);
static int TranslateArgs(TreeNode *node, FILE *fn);
static int TranslateBinaryOp(TreeNode *node, FILE *fn);
static int TranslateUnaryOp(TreeNode *node, FILE *fn);
static int TranslateNumber(TreeNode *node, FILE *fn);
static int TranslateVariable(TreeNode *node, FILE *fn);
static int TranslateString(TreeNode *node, FILE *fn);
static int TranslateKeyOp(TreeNode *node, FILE *fn);
static int TranslateCall(TreeNode *node, FILE *fn);
static int TranslatePunct(TreeNode *node, FILE *fn);
static int PrintPrototypes(TreeNode *node, FILE *fn);
static int PrintArgs(TreeNode *node, FILE *fn);

#undef KEY_OP

int TranslateTree(TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    FILE *fn = fileopen(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    if (TranslateFunction(tree->root, fn) != SUCCESS) {
        fileclose(fn);
        return ERROR;
    }

    fileclose(fn);

    return SUCCESS;
}

static int TranslateFunction(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    if (!IsValType(node, FUNCTION))
        return ERROR;

    if (!node->left)
        return ERROR;

    if (node->value.func_index == 0)
        fprintf(fn, "once upon the time () {\n\n");
    else {
        fprintf(fn, "farytale %d (", node->value.func_index);
        if (node->left->right)
            if (TranslateArgs(node->left->right, fn) != SUCCESS)
                return ERROR;
        fprintf(fn, ") {\n\n");
    }

    if (node->left->left)
        if (TranslateBody(node->left->left, fn) != SUCCESS)
            return ERROR;

    fprintf(fn, "}");

    return SUCCESS;

}

static int TranslateBody(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    switch (node->value.type) {
        case (BINARY_OP): {
            if (TranslateBinaryOp(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (UNARY_OP): {
            if (TranslateUnaryOp(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (NUMBER): {
            if (TranslateNumber(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (VARIABLE): {
            if (TranslateVariable(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (STRING): {
            if (TranslateString(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (KEY_OP): {
            if (TranslateKeyOp(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (FUNCTION): {
            if (TranslateCall(node, fn) != SUCCESS)
                return ERROR;
            break;
        }
        case (PUNCT_SYM): {
            if (TranslatePunct(node, fn) != SUCCESS)
                return ERROR;
        }
        default: {
            printf(RED "reverse frontend error: " END_OF_COLOR "invalid operation\n");
            return ERROR;
        }
    }
}

static int TranlateBinaryOP(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    if (!node->left || !node->right) {
        printf(RED "reverse frontend error: " END_OF_COLOR "right and left subtree expected for binary op\n");
        return ERROR;
    }

    if (TranslateBody(node->left, fn) != SUCCESS)
        return ERROR;

    fprintf(fn, " %s ", KEY_WORDS[node->value.bin_op]);

    if (TranslateBody(node->right, fn) != SUCCESS)
        return ERROR;
}

static int TranslateUnaryOp(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    if (!node->right) {
    }

    return SUCCESS;
}

static int TranslatePunct(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    if (node->value.type == NEW_LINE)
        fprintf(fn, ";\n");
    else {
        printf(RED "reverse frontend error: " END_OF_COLOR "invalid punct sym in node\n");
        return ERROR;
    }
}

static int PrintPrototypes(TreeNode *node, FILE *fn) {

    assert(node);
    assert(fn);

    if (node->value.type == FUNCTION && node->value.func_index == 0) {
        fprintf(fn, "faritale %s(");

        if (!node->left) return ERROR;
        if (PrintArgs(node->left->right, fn) != SUCCESS)
            return ERROR;

        fprintf(fn, ");\n");
    }

    if (node->left)
        if (PrintPrototypes(node->left, fn) != SUCCESS)
            return ERROR;

    if (node->right)
        if (PrintPrototypes(node->right, fn) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int PrintArgs(TreeNode *node, FILE *fn) {

    assert(fn);

    if (!node) return SUCCESS;

    if (!node->left) return ERROR;



    return SUCCESS;
}