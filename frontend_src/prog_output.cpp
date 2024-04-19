#include <stdio.h>
#include "prog_output.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"

static int printNode(TreeNode *node, Vector *names_table, FILE *fn);

int printInFile(TreeStruct *tree, Vector *names_table, const char *filename) {

    assert(tree);
    assert(filename);
    assert(names_table);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    FILE *fn = openFile(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    if (printNode(tree->root, names_table, fn) != SUCCESS)
        return ERROR;

    closeFile(fn);

    return SUCCESS;
}

static int printNode(TreeNode *node, Vector *names_table, FILE *fn) {

    assert(node);
    assert(fn);
    assert(names_table);

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
            fprintf(fn, "\"%s\" ", getStrPtr(names_table, node->value.var_index));
            break;
        }
        case (FUNCTION): {
            if (node->value.func_index == 0)
                fprintf(fn, "\"main\" ");
            else
                fprintf(fn, "\"%s\" ", getStrPtr(names_table, node->value.func_index));
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
        if (printNode(node->left, names_table, fn) != SUCCESS)
            return ERROR;
    }
    else {
        fprintf(fn, "_ ");
    }

    if (node->right) {
        if (printNode(node->right, names_table, fn) != SUCCESS)
            return ERROR;
    }
    else {
        fprintf(fn, "_ ");
    }

    fprintf(fn, ") ");

    return SUCCESS;
}