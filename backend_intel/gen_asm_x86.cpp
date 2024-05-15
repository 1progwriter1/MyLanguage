#include <stdio.h>
#include "gen_asm_x86.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../data/key_words_codes.h"
#include <stdlib.h>
#include "variables_x86.h"
#include "../lib_src/my_lang_lib.h"

#define CODE_GEN_ASSERT assert(node);                   \
                        assert(data);                   \
                        assert(data->fn);

enum VarSearchStatus {
    kStatusFound,
    kStatusNotFound,
};

static int genFunction(TreeNode *node, CodeGenData *data);
static int genNewLine(TreeNode *node, CodeGenData *data);
static int genIf(TreeNode *node, CodeGenData *data);
static int genWhile(TreeNode *node, CodeGenData *data);
static int genAssign(TreeNode *node, CodeGenData *data);
static int genOutput(TreeNode *node, CodeGenData *data);
static int genExpression(TreeNode *node, CodeGenData *data);
static int genLogicalJump(TreeNode *node, CodeGenData *data);
static int genUnaryOp(TreeNode *node, CodeGenData *data);
static int genBinaryOp(TreeNode *node, CodeGenData *data);
static int genInput(TreeNode *node, CodeGenData *data);
static int genCall(TreeNode *node, CodeGenData *data);
static int genRet(TreeNode *node, CodeGenData *data);

static void createStart(FILE *fn);

int genAsmCode(TreeStruct *tree, Vector *names_table, const char *filename) {

    assert(tree);
    assert(filename);
    assert(names_table);

    if (treeVerify(tree) != SUCCESS)
        return ERROR;

    if (tree->root->value.type != FUNCTION) {
        printf(RED "asm gen error: " END_OF_COLOR "function expected\n");
        return ERROR;
    }

    struct Vector variables = {};
    struct CodeGenData data = {NULL, data.vars.variables = &variables, data.vars.names_table = names_table};
    if (prepareData(&data, filename, names_table) != SUCCESS)
        return ERROR;

    createStart(data.fn);

    if (genFunction(tree->root, &data) != SUCCESS)
        return ERROR;

    dtorData(&data);

    return SUCCESS;
}

static int genFunction(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.func_index == 0) {
        fprintf(data->fn, "main:\n");
    }
    else {
        fprintf(data->fn, "%s:\n", getStrPtr(data->vars.names_table, node->value.func_index));
    }
    setRbp(data);

    if (!isPunct(node->left, NEW_LINE)) {
        printf(RED "gen asm error: " END_OF_COLOR "function body expected\n");
        return ERROR;
    }

    if (createSegment(data, node->left->right) != SUCCESS)
        return ERROR;

    if (isPunct(node->left->left, NEW_LINE))
        if (genNewLine(node->left->left, data) != SUCCESS) return ERROR;

    if (destroySegment(data) != SUCCESS) return ERROR;

    if (isType(node->right, FUNCTION))
        if (genFunction(node->right, data) != SUCCESS) return ERROR;

    return SUCCESS;
}

static int genNewLine(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left)
        return SUCCESS;

    if (isKeyOp(node->left, IF)) {
        if (genIf(node->left, data) != SUCCESS)
            return ERROR;
    }
    else if (isKeyOp(node->left, WHILE)) {
        if (genWhile(node->left, data) != SUCCESS)
            return ERROR;
    }
    else if (isBinOp(node->left, ASSIGN)) {
        if (genAssign(node->left, data) != SUCCESS)
            return ERROR;
    }
    else if (isUnOp(node->left, OUT) || isUnOp(node->left, OUT_S)) {
        if (genOutput(node->left, data) != SUCCESS)
            return ERROR;
    }
    else if (isUnOp(node->left, IN)) {
        if (genInput(node->left, data) != SUCCESS)
            return ERROR;
    }
    else if (isType(node->left, FUNCTION)) {
        saveRdxRcx(data);
        if (genCall(node->left->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall %s\n", getStrPtr(data->vars.names_table, node->left->value.func_index));
        restoreRdxRcx(data);
    }

    else if (isUnOp(node->left, RET))
        if (genRet(node->left, data) != SUCCESS)
            return ERROR;

    if (isPunct(node->right, NEW_LINE))
        if (genNewLine(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int genRet(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->right) {
        if (genExpression(node->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tpop rax\n");
    }

    fprintf(data->fn,   "\t\tpop rbp\n"
                        "\t\tret\n\n");

    return SUCCESS;
}

static int genInput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right || node->right->value.type != VARIABLE) {
        printf(RED "gen asm error: " END_OF_COLOR "variable for input expected\n");
        return ERROR;
    }

    fprintf(data->fn, "\t\tin\n");

    if (writeVariable(data, node->right, {}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int genCall(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!isType(node, VARIABLE))  return SUCCESS;

    if (node->right) genCall(node->right, data);

    getVariableValue(data, node->value.var_index);

    return SUCCESS;
}

static int genIf(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->left->left || !node->left->right) return ERROR;

    if (genExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data) != SUCCESS)  return ERROR;

    size_t if_index = data->indexes.cur_if++;

    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "if_%lu\n", if_index);

    if (node->right->right && isPunct(node->right->right, NEW_LINE))
        if (genNewLine(node->right->right, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp end_if_%lu\n", if_index);
    fprintf(data->fn, ":if_%lu\n", if_index);

    if (node->right->left && isPunct(node->right->left, NEW_LINE))
        if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, ":end_if_%lu\n", if_index);

    return SUCCESS;
}

static int genWhile(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right) {
        printf(RED "gen asm error: " END_OF_COLOR "incorrect while struct\n");
        return ERROR;
    }
    size_t while_index = data->indexes.cur_while++;
    fprintf(data->fn, ":while_%lu\n", while_index);

    if (genExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data) != SUCCESS) return ERROR;

    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "begin_while_%lu\n", while_index);

    fprintf(data->fn, "\t\tjmp end_while_%lu\n", while_index);

    fprintf(data->fn, ":begin_while_%lu\n", while_index);
    if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp while_%lu\n", while_index);

    fprintf(data->fn, ":end_while_%lu\n", while_index);

    return SUCCESS;
}

static int genAssign(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!isType(node->left, VARIABLE) || !node->right)
        return ERROR;

    if (isType(node->right, NUMBER)) {
        if (writeVariable(data, node->left, (ValueSrc) {.type = TypeNumber, .number = (long long) node->right->value.number}) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }
    if (genExpression(node->right, data) != SUCCESS)
        return ERROR;

    if (writeVariable(data, node->left, {}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int genOutput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right) return ERROR;

    Unary_Op operation = node->value.un_op;

    if (operation == OUT_S) {

        char *str = node->right->value.string;
        while (*str != '\0') {
            if (*str == '\\' && *(str + 1) == 'n') {
                fprintf(data->fn, "\t\tpush %d\n\t\toutc\n", '\n');
                str += 2;
            }
            else fprintf(data->fn, "\t\tpush %d\n\t\toutc\n", *str++);
        }
    }
    else {
        if (genExpression(node->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tout\n");
    }

    return SUCCESS;
}

static int genExpression(TreeNode *node, CodeGenData *data) {

    assert(data);
    assert(data->fn);

    if (!node)
        return SUCCESS;

    if (isType(node, UNARY_OP)) {
        if (genExpression(node->right, data) != SUCCESS)
            return ERROR;

        if (genUnaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }
    if (isType(node, BINARY_OP)) {

        if (genExpression(node->left, data) != SUCCESS || genExpression(node->right, data) != SUCCESS)
                return ERROR;

        if (genBinaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }

    if (isType(node, VARIABLE)) {
        if (getVariableValue(data, node->value.var_index) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }
    if (isType(node, NUMBER)) {
        fprintf(data->fn, "\t\tpush %lg\n", node->value.number);
        return SUCCESS;
    }

    if (isType(node, FUNCTION)) {
        saveRdxRcx(data);
        if (genCall(node->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall %s\n", getStrPtr(data->vars.names_table, node->value.func_index));
        restoreRdxRcx(data);
        fprintf(data->fn, "\t\tpush rax\n");
        return SUCCESS;
    }

    printf(RED "asm gen error: " END_OF_COLOR "invalid value type for expression\n");

    return ERROR;

}

static void createStart(FILE *fn) {

    assert(fn);

    fprintf(fn, "section .text\n\n"
                "global _start\n\n"
                "_start:\n"
                "\t\tcall main\n"
                "\t\tmov rax, 0x3C\t\t;exit\n"
                "\t\txor rdi, rdi\n"
                "\t\tsyscall\n\n");
}

static int genLogicalJump(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != BINARY_OP) {
        printf(RED "gen asm error: " END_OF_COLOR "binary operator expected\n");
        return ERROR;
    }

    if (node->value.type != BINARY_OP)
        return ERROR;

    if (node->value.bin_op == EQUAL)
        fprintf(data->fn, "\t\tje ");

    else if (node->value.bin_op == NOT_EQ)
        fprintf(data->fn, "\t\tjne ");

    else if (node->value.bin_op == ABOVE)
        fprintf(data->fn, "\t\tja ");

    else if (node->value.bin_op == BELOW)
        fprintf(data->fn, "\t\tjb ");

    else if (node->value.bin_op == AB_EQ)
        fprintf(data->fn, "\t\tjae ");

    else if (node->value.bin_op == BEl_EQ)
        fprintf(data->fn, "\t\tjbe ");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid logical operation\n");
        return ERROR;
    }

    return SUCCESS;
}

static int genUnaryOp(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != UNARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "unary operation expected\n");
        return ERROR;
    }

    if (node->value.un_op == SIN)
        fprintf(data->fn, "\t\tsin\n");

    else if (node->value.un_op == COS)
        fprintf(data->fn, "\t\tcos\n");

    else if (node->value.un_op == LN)
        fprintf(data->fn, "\t\tln\n");

    else if (node->value.un_op == SQRT)
        fprintf(data->fn, "\t\tsqrt\n");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid unary operation\n");
        return ERROR;
    }

    return SUCCESS;
}

static int genBinaryOp(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != BINARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "binary operation expected\n");
        return ERROR;
    }

    if (node->value.bin_op == ADD)
        fprintf(data->fn, "\t\tadd\n");

    else if (node->value.bin_op == SUB)
        fprintf(data->fn, "\t\tsub\n");

    else if (node->value.bin_op == MUL)
        fprintf(data->fn, "\t\tmul\n");

    else if (node->value.bin_op == DIV)
        fprintf(data->fn, "\t\tdiv\n");

    else if (node->value.bin_op == POW)
        fprintf(data->fn, "\t\tpow\n");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid binary operation\n");
        return ERROR;
    }

    return SUCCESS;

}

bool isPunct(TreeNode *node, Punctuation sym) {

    return node && node->value.type == PUNCT_SYM && node->value.sym_code == sym;
}

bool isBinOp(TreeNode *node, Binary_Op operation) {

    return node && node->value.type == BINARY_OP && node->value.bin_op == operation;
}

bool isUnOp(TreeNode *node, Unary_Op operation) {

    return node && node->value.type == UNARY_OP && node->value.un_op == operation;
}

bool isKeyOp(TreeNode *node, Key_Op operation) {

    return node && node->value.type == KEY_OP && node->value.key_op == operation;
}

bool isType(TreeNode *node, ValueType type) {

    return node && node->value.type == type;
}