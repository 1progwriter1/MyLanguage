#include <stdio.h>
#include "../headers/gen_asm.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/key_words_codes.h"

#define CODE_GEN_ASSERT assert(node);       \
                        assert(data);       \
                        assert(data->fn);

static int GenFunction(TreeNode *node, CodeGenData *data);
static int GenNewLine(TreeNode *node, CodeGenData *data);
static int GenIf(TreeNode *node, CodeGenData *data);
static int GenWhile(TreeNode *node, CodeGenData *data);
static int GenAssign(TreeNode *node, CodeGenData *data);
static int GenOutput(TreeNode *node, CodeGenData *data);

static void CallMainPrint(FILE *fn);

static bool IsPunct(TreeNode *node, Punctuation sym);
static bool IsBinOp(TreeNode *node, Binary_Op operation);
static bool IsUnOp(TreeNode *node, Unary_Op operation);
static bool IsKeyOp(TreeNode *node, Key_Op operation);

int GenAsmCode(TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    if (tree->root->value.type != FUNCTION) {
        printf(RED "asm gen error: " END_OF_COLOR "function expected\n");
        return ERROR;
    }

    FILE *fn = fileopen(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    CallMainPrint(fn);

    CodeGenData data = {fn, 0, 0};
    if (GenFunction(tree->root, &data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int GenFunction(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.func_index == 0) {
        fprintf(data->fn, ":main\n");
    }
    else {
        fprintf(data->fn, ":func_%lu\n", data->cur_func++);
    }

    if (node->left && IsPunct(node->left, NEW_LINE)) {
        if (GenNewLine(node->left, data) != SUCCESS)
            return ERROR;
    }
    fprintf(data->fn, "\t\tret\n\n");

    if (node->right && node->right->value.type == FUNCTION)
        if (GenFunction(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int GenNewLine(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left)
        return SUCCESS;

    if (IsKeyOp(node->left, IF))
        if (GenIf(node->left, data) != SUCCESS)
            return ERROR;

    if (IsKeyOp(node->left, WHILE))
        if (GenWhile(node->left, data) != SUCCESS)
            return ERROR;

    if (IsBinOp(node->left, ASSIGN))
        if (GenAssign(node->left, data) != SUCCESS)
            return ERROR;

    if (IsUnOp(node->left, OUT) || IsUnOp(node->left, OUT_S))
        if (GenOutput(node->left, data) != SUCCESS)
            return ERROR;

    if (node->right)
        if (IsPunct(node->right, NEW_LINE))
            if (GenNewLine(node->right, data) != SUCCESS)
                return ERROR;

    return SUCCESS;
}

static int GenIf(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    return SUCCESS;
}

static int GenWhile(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    return SUCCESS;
}

static int GenAssign(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    return SUCCESS;
}

static int GenOutput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    return SUCCESS;
}

static void CallMainPrint(FILE *fn) {

    assert(fn);

    fprintf(fn, "\t\tcall main\n\t\thlt\n\n");
}

static bool IsPunct(TreeNode *node, Punctuation sym) {

    assert(node);

    return node->value.type == PUNCT_SYM && node->value.sym_code == sym;
}

static bool IsBinOp(TreeNode *node, Binary_Op operation) {

    assert(node);

    return node->value.type == BINARY_OP && node->value.bin_op == operation;
}

static bool IsUnOp(TreeNode *node, Unary_Op operation) {

    assert(node);

    return node->value.type == UNARY_OP && node->value.un_op == operation;
}

static bool IsKeyOp(TreeNode *node, Key_Op operation) {

    assert(node);

    return node->value.type == KEY_OP && node->value.key_op == operation;
}