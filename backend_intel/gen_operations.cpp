#include <stdio.h>
#include "gen_operations.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include <stdlib.h>

int genDiv(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    CODE_GEN_ASSERT

    ValueSrc left = {};
    if (genExpression(node->left, data, &left) != SUCCESS)
        return ERROR;

    ValueSrc right = {};
    if (genExpression(node->right, data, &right) != SUCCESS)
        return ERROR;

    fprintf(data->fn, "\t\tmov rax, ");
    printPlace(data, left);
    fprintf(data->fn, "\n\t\tcqo\n");

    if (right.type == TypeNumber)
        if (moveToRegister(data, &right) != SUCCESS)
            return ERROR;

    fprintf(data->fn, "\t\tidiv qword ");
    printPlace(data, right);
    fprintf(data->fn, "\n");

    src->type = TypeReg;
    src->reg = RAX;

    return SUCCESS;
}

int genMul(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    ValueSrc left = {};
    if (genExpression(node->left, data, &left) != SUCCESS)
        return ERROR;

    ValueSrc right = {};
    if (genExpression(node->right, data, &right) != SUCCESS)
        return ERROR;

    if (left.type != TypeReg) {
        if (moveToRegister(data, &left) != SUCCESS)
            return ERROR;
    }

    fprintf(data->fn, "\t\timul ");
    printPlace(data, left);
    fprintf(data->fn, ", ");
    printPlace(data, right);
    fprintf(data->fn, "\n");

    *src = left;

    return SUCCESS;
}

int genAdd(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    ValueSrc left = {};
    if (genExpression(node->left, data, &left) != SUCCESS)
        return ERROR;

    ValueSrc right = {};
    if (genExpression(node->right, data, &right) != SUCCESS)
        return ERROR;

    if (left.type != TypeReg) {
        if (right.type != TypeReg) {
            if (moveToRegister(data, &left) != SUCCESS)
                return ERROR;
        }
        else {
            ValueSrc tmp = right;
            right = left;
            left = tmp;
        }
    }

    fprintf(data->fn, "\t\tadd ");
    printPlace(data, left);
    fprintf(data->fn, ", ");
    printPlace(data, right);
    fprintf(data->fn, "\n");

    *src = left;

    return SUCCESS;
}

int genSub(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    ValueSrc left = {};
    if (genExpression(node->left, data, &left) != SUCCESS)
        return ERROR;

    ValueSrc right = {};
    if (genExpression(node->right, data, &right) != SUCCESS)
        return ERROR;

    if (left.type != TypeReg) {
        if (moveToRegister(data, &left) != SUCCESS)
            return ERROR;
    }

    fprintf(data->fn, "\t\tsub ");
    printPlace(data, left);
    fprintf(data->fn, ", ");
    printPlace(data, right);
    fprintf(data->fn, "\n");

    *src = left;

    return SUCCESS;
}

int genOutput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right) return ERROR;

    Unary_Op operation = node->value.un_op;

    if (operation == OUT_S) {

        char *str = node->right->value.string;
        if (!str)   return ERROR;

        if (pushBack(data->vars.str_data, &str) != SUCCESS)
            return ERROR;

        fprintf(data->fn, "\t\tmov rdi, STR%lu\t\t;print %s\n", data->vars.str_data->size - 1, str);
        fprintf(data->fn, "\t\tcall my_printf\n");
    }
    else {
        char *str = (char *) calloc (sizeof("%d"), sizeof(char));
        if (!str)   return ERROR;
        str[0] = '%'; str[1] = 'd';
        if (pushBack(data->vars.str_data, &str) != SUCCESS)
            return ERROR;

        ValueSrc src = {};
        if (genExpression(node->right, data, &src) != SUCCESS) {
            free(str);
            return ERROR;
        }
        fprintf(data->fn, "\t\tmov rdi, STR%lu\n"
                           "\t\tmov rsi, ", data->vars.str_data->size - 1);
        printPlace(data, src);
        fprintf(data->fn, "\n\t\tcall my_printf\t\t;print number\n");
    }

    return SUCCESS;
}

int moveToRegister(CodeGenData *data, ValueSrc *src) {

    assert(data);
    assert(src);

    int reg = findFreeRegister(data);
    if (reg < (int) RAX || reg > (int) R15) {
        printf(RED "generation error: " END_OF_COLOR "can't find free register\n");
        return ERROR;
    }
    data->used_regs[reg].is_used = true;
    fprintf(data->fn, "\t\tmov %s, ", REGS_NAMES[reg]);
    printPlace(data, *src);
    fprintf(data->fn, "\n");
    src->type = TypeReg;
    src->reg = (Registers) reg;

    return SUCCESS;
}