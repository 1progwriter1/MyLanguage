#include <stdio.h>
#include "../headers/prog_output.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"

static int PrintNode(TreeNode *node, NamesTable *data, FILE *fn);

int PrintInFile(TreeStruct *tree, NamesTable *data, const char *filename) {

    assert(tree);
    assert(filename);
    assert(data);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    FILE *fn = fileopen(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    if (PrintNode(tree->root, data, fn) != SUCCESS)
        return ERROR;

    fileclose(fn);

    return SUCCESS;
}

static int PrintNode(TreeNode *node, NamesTable *data, FILE *fn) {

    assert(node);
    assert(fn);
    assert(data);

    fprintf(fn, "( ");

    fprintf(fn, "%d ", node->value.type);

    switch (node->value.type) {
        case (PUNCT_SYM): {
            fprintf(fn, "%d ", node->value.sym_code);
            break;
        }
        case (BINARY_OP): {
            fprintf(fn, "%d ", node->value.bin_op);
            break;
        }
        case (UNARY_OP): {
            fprintf(fn, "%d ", node->value.un_op);
            break;
        }
        case (KEY_OP): {
            fprintf(fn, "%d ", node->value.key_op);
            break;
        }
        case (NUMBER): {
            fprintf(fn, "%lg ", node->value.number);
            break;
        }
        case (VARIABLE): {
            fprintf(fn, "\"%s\" ", data->names[node->value.var_index].name);
            break;
        }
        case (FUNCTION): {
            if (node->value.func_index == 0)
                fprintf(fn, "\"main\" ");
            else
                fprintf(fn, "\"%s\" ", data->names[node->value.func_index].name);
            break;
        }
        case (STRING): {
            fprintf(fn, "\"%s\" ", node->value.string);
            break;
        }
        default: {
            printf(RED "print error: " END_OF_COLOR "invalid value type\n");
            return ERROR;
        }
    }

    if (node->left) {
        if (PrintNode(node->left, data, fn) != SUCCESS)
            return ERROR;
    }
    else {
        fprintf(fn, "_ ");
    }

    if (node->right) {
        if (PrintNode(node->right, data, fn) != SUCCESS)
            return ERROR;
    }
    else {
        fprintf(fn, "_ ");
    }

    fprintf(fn, ") ");

    return SUCCESS;
}