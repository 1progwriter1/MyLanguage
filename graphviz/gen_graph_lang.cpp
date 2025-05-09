#include <stdio.h>
#include "file_func.h"
#include "gen_graph_lang.h"
#include <assert.h>
#include "../data/key_words_codes.h"
#include "systemdata.h"
#include <stdlib.h>

const size_t MAX_CMD_LEN = 60;

const char *TYPES[]  = {"punct_sym", "binary_op", "unary_op", "key_op", "number", "variable", "function", "string"};
const char *VALUES[] = {"sin", "cos", "sqrt", "ln", "not", "out", "out_s", "in", "call", "ret", "+", "-",
                        "mul", "\\\\", "^", "==", "stronger", "weaker", "not weaker", "not stronger", "!=", "=", "if", "while", ";"};

static int genNodes(FILE *fn, const TreeNode *node, size_t *index, Vector *names_table);
static int printNode(FILE *fn, const TreeNode *node, const size_t index, Vector *names_table);
static const char *getStrPtr(Vector *names_table, size_t index);

int genGraphLang(const TreeStruct *tree, const char *filename, Vector *names_table) {

    assert(tree);
    assert(filename);

    FILE *fn = openFile(GRAPH_SRC, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    size_t index = 0;
    fprintf(fn, "digraph G {\n\t");
    fprintf(fn, "%d [shape = \"polygon\", label = \"size: %lu\\naddress: %p\", style = \"filled\", color = \"#FFFF99\"]\n\t-10->0 [weight = 1000, color = \"#FFFFFF\"]\n\t", -10, tree->size, tree->root);

    if (!tree->root)
        return NULL_POINTER;

    genNodes(fn, tree->root, &index, names_table);

    fprintf(fn, "\n");
    fprintf(fn, "}");

    closeFile(fn);

    char command[MAX_CMD_LEN] = "";

    snprintf(command, MAX_CMD_LEN ,"dot " GRAPH_SRC " -T png -o %s", filename);
    system(command);

    return SUCCESS;
}

static int genNodes(FILE *fn, const TreeNode *node, size_t *index, Vector *names_table) {

    assert(fn);
    assert(node);
    assert(index);
    assert(names_table);

    printNode(fn, node, *index, names_table);
    size_t main_index = *index;

    if (node->left) {
        *index += 1;
        fprintf(fn, "%lu->%lu\n\t", main_index, *index);
        genNodes(fn, node->left, index, names_table);
    }
    if (node->right) {
        *index += 1;
        fprintf(fn, "%lu->%lu\n\t", main_index, *index);
        genNodes(fn, node->right, index, names_table);
    }

    return SUCCESS;
}

static int printNode(FILE *fn, const TreeNode *node, const size_t index, Vector *names_table) {

    assert(fn);
    assert(node);
    assert(names_table);

    fprintf(fn, "%lu [shape = Mrecord, style = filled, fillcolor = \"#00FFFF\", color = \"#331900\", label = \"%s: ", index, TYPES[node->value.type]);

    switch (node->value.type) {
        case (PUNCT_SYM): {
            fprintf(fn, "%s", VALUES[node->value.sym_code]);
            break;
        }
        case (BINARY_OP): {
            fprintf(fn, "%s", VALUES[node->value.bin_op]);
            break;
        }
        case (UNARY_OP): {
            fprintf(fn, "%s", VALUES[node->value.un_op]);
            break;
        }
        case (KEY_OP): {
            fprintf(fn, "%s", VALUES[node->value.key_op]);
            break;
        }
        case (NUMBER): {
            fprintf(fn, "%lg", node->value.number);
            break;
        }
        case (VARIABLE): {
            fprintf(fn, "%s", getStrPtr(names_table, node->value.var_index));
            break;
        }
        case (FUNCTION): {
            if (node->value.func_index == 0)    fprintf(fn, "main");
            else fprintf(fn, "%s", getStrPtr(names_table, node->value.func_index));
            break;
        }
        case (STRING): {
            fprintf(fn, "%s", node->value.string);
            break;
        }
        default: {
            printf(RED "graph error: " END_OF_COLOR "incorrect type\n");
            return ERROR;
        }
    }

    fprintf(fn, "\"]\n\t");

    return SUCCESS;
}

static const char *getStrPtr(Vector *names_table, size_t index) {

    assert(names_table);

    return ((Name *) ((char *) names_table->data + index * names_table->element_size))->name;
}
