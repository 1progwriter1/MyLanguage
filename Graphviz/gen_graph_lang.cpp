#include <stdio.h>
#include "../../MyLibraries/headers/file_func.h"
#include "../diff_func/headers/diff_values.h"
#include "gen_graph_lang.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"

const size_t MAX_CMD_LEN = 60;

static int GenDiffNodes(FILE *fn, const TreeNode *node, size_t *index);
static int PrintNodeTree(FILE *fn, const TreeNode *node, const size_t index);

int GenGraphDiff(const TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    FILE *fn = fileopen(GRAPH_SRC, WRITE);
    if (!fn)
        return FILE_OPEN_ERROR;

    size_t index = 0;
    fprintf(fn, "digraph G {\n\t");
    fprintf(fn, "%d [shape = \"polygon\", label = \"size: %lu\\naddress: %p\", style = \"filled\", color = \"#FFFF99\"]\n\t-10->0 [weight = 1000, color = \"#FFFFFF\"]\n\t", -10, tree->size, tree->root);

    if (!tree->root)
        return NULL_POINTER;

    GenDiffNodes(fn, tree->root, &index);

    fprintf(fn, "\n");
    fprintf(fn, "}");

    fileclose(fn);

    char command[MAX_CMD_LEN] = "";

    snprintf(command, MAX_CMD_LEN ,"dot " GRAPH_SRC " -T png -o %s", filename);
    system(command);

    return SUCCESS;
}

static int GenDiffNodes(FILE *fn, const TreeNode *node, size_t *index) {

    assert(fn);
    assert(node);
    assert(index);

    PrintNodeTree(fn, node, *index);
    size_t main_index = *index;

    if (node->left) {
        *index += 1;
        fprintf(fn, "%lu->%lu\n\t", main_index, *index);
        GenDiffNodes(fn, node->left, index);
    }
    if (node->right) {
        *index += 1;
        fprintf(fn, "%lu->%lu\n\t", main_index, *index);
        GenDiffNodes(fn, node->right, index);
    }

    return SUCCESS;
}

static int PrintNodeTree(FILE *fn, const TreeNode *node, const size_t index) {

    assert(fn);
    assert(node);

    fprintf(fn, "%lu [shape = Mrecord, style = filled, fillcolor = \"", index);

    #define DEF_OP(name, code, sym, ...)    \
        case (code): {                      \
            fprintf(fn, sym);               \
            break;                          \
        }

    switch (node->value.type) {
        case (OPERATION): {
            fprintf(fn, "#00FFFF\", color = \"#331900\", label = \"");
            switch (node->value.operation) {
                #include "../diff_func/headers/operations.h"
                default: {
                    printf(RED "Incorrect operation number" END_OF_COLOR "\n");
                    return ERROR;
                }
            }
            break;
        }
        case (NUMBER): {
            fprintf(fn, "#66FF66\", color = \"#331900\", label = \"%lg", node->value.number);
            break;
        }
        case (VARIABLE): {
            fprintf(fn, "#FFFF66\", color = \"#331900\", label = \"x");
            break;
        }
        default: {
            printf(RED "Incorect value type" END_OF_COLOR "\n");
            return ERROR;
        }
    }

    #undef DEF_OP

    return SUCCESS;
}