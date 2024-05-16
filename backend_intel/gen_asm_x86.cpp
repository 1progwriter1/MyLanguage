#include <stdio.h>
#include "gen_asm_x86.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../data/key_words_codes.h"
#include <stdlib.h>
#include "variables_x86.h"
#include "../lib_src/my_lang_lib.h"
#include <string.h>

enum VarSearchStatus {
    kStatusFound,
    kStatusNotFound,
};

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

    Vector variables = {};
    Vector str_data = {};
    CodeGenData data = {NULL, 0, {}, {}, data.vars.variables = &variables, data.vars.names_table = names_table, data.vars.str_data = &str_data};
    if (prepareData(&data, filename, names_table) != SUCCESS)
        return ERROR;

    createStart(data.fn);

    if (genFunction(tree->root, &data) != SUCCESS)
        return ERROR;

    createRoData(&data);

    dtorData(&data);

    return SUCCESS;
}

int genFunction(TreeNode *node, CodeGenData *data) {

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

int genNewLine(TreeNode *node, CodeGenData *data) {

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
        if (genCall(node->left->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall %s\n", getStrPtr(data->vars.names_table, node->left->value.func_index));
    }

    else if (isUnOp(node->left, RET))
        if (genRet(node->left, data) != SUCCESS)
            return ERROR;

    setRegisters(data);

    if (isPunct(node->right, NEW_LINE))
        if (genNewLine(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

int genRet(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->right) {
        ValueSrc src = {};
        if (genExpression(node->right, data, &src) != SUCCESS)
            return ERROR;

        if (src.type != TypeReg || src.reg != RAX) {
            fprintf(data->fn, "\t\tmov rax, ");
            if (src.type == TypeNumber)     fprintf(data->fn, "%lld\t\t;pass the argument\n", src.number);
            else if (src.type == TypeStack) fprintf(data->fn, "[rbx - %lu]\t\t;pass the argument\n", (src.index + 1) * VALUE_SIZE);
            else                            fprintf(data->fn, "%s\t\t;pass the argument\n", REGS_NAMES[src.reg]);
        }
    }

    fprintf(data->fn,   "\t\tleave\n"
                        "\t\tret\n\n");

    return SUCCESS;
}

int genInput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right || node->right->value.type != VARIABLE) {
        printf(RED "gen asm error: " END_OF_COLOR "variable for input expected\n");
        return ERROR;
    }

    fprintf(data->fn, "\t\tcall my_scanf\t\t;read number to rax\n");

    ValueSrc src = {.type = TypeReg, .reg = RAX};
    if (writeVariable(data, node->right, src) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

int genCall(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    TreeNode *cur = node;
    size_t cur_arg = 0;
    while (isType(cur, VARIABLE)) {
        fprintf(data->fn, "\t\tmov %s, ", REGS_NAMES[ARGUMENTS_SRC[cur_arg++]]);
        if (getVariableValue(data, cur->value.var_index) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\n");
        cur = cur->right;
    }

    return SUCCESS;
}

int genIf(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->left->left || !node->left->right) return ERROR;

    ValueSrc left = {};
    ValueSrc right = {};
    if (genExpression(node->left->right, data, &left) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data, &right) != SUCCESS)  return ERROR;

    size_t if_index = data->indexes.cur_if++;

    fprintf(data->fn, "\t\tcmp ");
    printPlace(data, right);
    fprintf(data->fn, ", ");
    printPlace(data, left);
    fprintf(data->fn, "\n");

    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, ".if_%lu\n", if_index);

    if (node->right->right && isPunct(node->right->right, NEW_LINE))
        if (genNewLine(node->right->right, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp .end_if_%lu\n", if_index);
    fprintf(data->fn, ".if_%lu:\n", if_index);

    if (node->right->left && isPunct(node->right->left, NEW_LINE))
        if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, ".end_if_%lu:\n", if_index);

    return SUCCESS;
}

int genWhile(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right) {
        printf(RED "gen asm error: " END_OF_COLOR "incorrect while struct\n");
        return ERROR;
    }
    size_t while_index = data->indexes.cur_while++;
    fprintf(data->fn, ":while_%lu\n", while_index);

    ValueSrc left = {};
    ValueSrc right = {};
    if (genExpression(node->left->right, data, &left) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data, &right) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tcmp ");
    printPlace(data, right);
    fprintf(data->fn, ", ");
    printPlace(data, left);
    fprintf(data->fn, "\n");
    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, ".begin_while_%lu\n", while_index);

    fprintf(data->fn, "\t\tjmp .end_while_%lu\n", while_index);

    fprintf(data->fn, ".begin_while_%lu:\n", while_index);
    if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp .while_%lu\n", while_index);

    fprintf(data->fn, ".end_while_%lu:\n", while_index);

    return SUCCESS;
}

int genAssign(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!isType(node->left, VARIABLE) || !node->right)
        return ERROR;

    ValueSrc src = {};
    if (genExpression(node->right, data, &src) != SUCCESS)
        return ERROR;

    if (writeVariable(data, node->left, src) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

int genExpression(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    assert(data);
    assert(data->fn);
    assert(src);

    if (!node)
        return SUCCESS;

    if (isType(node, BINARY_OP)) {
        if (genBinaryOp(node, data, src) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }

    else if (isType(node, VARIABLE)) {
        size_t index = 0;
        if (!findVariable(data, &index, node->value.var_index)) {
            printf(RED "error: " END_OF_COLOR "variable was not defined\n");
            return ERROR;
        }
        *src = {.type = TypeStack, index};
        return SUCCESS;
    }
    else if (isType(node, NUMBER)) {
        *src = {.type = TypeNumber, .number = (long long) node->value.number};
        return SUCCESS;
    }

    else if (isType(node, FUNCTION)) {
        if (genCall(node->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall %s\n", getStrPtr(data->vars.names_table, node->value.func_index));
        *src = {.type = TypeReg, .reg = RAX};
        return SUCCESS;
    }
    else if (isType(node, UNARY_OP)) {
        if (genUnaryOp(node, data, src) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }

    printf(RED "asm gen error: " END_OF_COLOR "invalid value type for expression\n");

    return ERROR;

}

void createStart(FILE *fn) {

    assert(fn);

    fprintf(fn, "section .text\n\n"
                "extern my_printf\n"
                "extern my_scanf\n\n"
                "global _start\n\n"
                "_start:\n"
                "\t\tcall main\n"
                "\t\tmov rax, 0x3C\t\t;exit\n"
                "\t\txor rdi, rdi\n"
                "\t\tsyscall\n\n");
}

int genLogicalJump(TreeNode *node, CodeGenData *data) {

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
        fprintf(data->fn, "\t\tjg ");

    else if (node->value.bin_op == BELOW)
        fprintf(data->fn, "\t\tjl ");

    else if (node->value.bin_op == AB_EQ)
        fprintf(data->fn, "\t\tjge ");

    else if (node->value.bin_op == BEl_EQ)
        fprintf(data->fn, "\t\tjle ");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid logical operation\n");
        return ERROR;
    }

    return SUCCESS;
}

int genBinaryOp(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    if (node->value.type != BINARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "binary operation expected\n");
        return ERROR;
    }

    if (node->value.bin_op == ADD) {
        if (genAdd(node, data, src) != SUCCESS)
            return ERROR;
    }
    else if (node->value.bin_op == SUB) {
        if (genSub(node, data, src) != SUCCESS)
            return ERROR;
    }
    else if (node->value.bin_op == MUL) {
        if (genMul(node, data, src) != SUCCESS)
            return ERROR;
    }
    else if (node->value.bin_op == DIV) {
        if (genDiv(node, data, src) != SUCCESS)
            return ERROR;
    }
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

void createRoData(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, "section .rodata\n");
    for (size_t i = 0; i < data->vars.str_data->size; i++) {
        fprintf(data->fn, "STR%lu:\n\t\tdb ", i);
        printStr(data, *((char **) getPtr(data->vars.str_data, i)));
        if (*((char **) getPtr(data->vars.str_data, i))[0] == '%') free(*((char **) getPtr(data->vars.str_data, i)));
    }
}

void printStr(CodeGenData *data, char *str) {

    assert(data);
    assert(str);

    fprintf(data->fn, "\"");
    while (*str != '\0') {
        if (*str == '\\' && *(str + 1) == 'n') {
            fprintf(data->fn, "\", 0xA, \"");
            str += 2;
        }
        else fprintf(data->fn, "%c", *str++);
    }
    fprintf(data->fn, "\", 0x0\n");
}

int genUnaryOp(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    if (node->value.type != UNARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "unary operation expected\n");
        return ERROR;
    }

    if (node->value.un_op == SQRT) {
        if (genSqrt(node, data, src) != SUCCESS)
            return ERROR;
    }

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid unary operation\n");
        return ERROR;
    }

    return SUCCESS;
}