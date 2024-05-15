#include <stdio.h>
#include "gen_operations.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"

int genSub(TreeNode *node, CodeGenData *data, ValueSrc *src) {

    CODE_GEN_ASSERT

    ValueSrc left = {};
    if (genExpression(node->left, data, &left) != SUCCESS)
        return ERROR;

    ValueSrc right = {};
    if (genExpression(node->right, data, &right) != SUCCESS)
        return ERROR;

    if (left.type == TypeStack) {
        int reg = findFreeRegister(data);
        if (reg < 0 || reg > 15) {
            printf(RED "generation error: " END_OF_COLOR "can't find free register\n");
            return ERROR;
        }
        fprintf(data->fn, "\t\tmov %s, ", REGS_NAMES[reg]);
        printPlace(data, left);
        fprintf(data->fn, "\n");
        left.type = TypeReg;
        left.reg = (Registers) reg;
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

        if (pushBack(data->str_data, &str) != SUCCESS)
            return ERROR;

        fprintf(data->fn, "\t\tmov rdi, STR%lu\t\t;print %s\n", data->str_data->size - 1, str);
        fprintf(data->fn, "\t\tcall my_printf\n");
    }
    else {
        char *str = (char *) calloc (sizeof("%d"), sizeof(char));
        if (!str)   return ERROR;
        str[0] = '%'; str[1] = 'd';
        if (pushBack(data->str_data, &str) != SUCCESS)
            return ERROR;

        ValueSrc src = {};
        if (genExpression(node->right, data, &src) != SUCCESS) {
            free(str);
            return ERROR;
        }
        fprintf(data->fn, "\t\tmov rdi, STR%lu\n"
                           "\t\tmov rsi, ", data->str_data->size - 1);
        printPlace(data, src);
        fprintf(data->fn, "\n\t\tcall my_printf\t\t;print number\n");
    }

    return SUCCESS;
}