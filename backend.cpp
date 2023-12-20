#include <stdio.h>
#include <assert.h>
#include "../MyLibraries/headers/systemdata.h"
#include "Graphviz/gen_graph_lang.h"
#include "headers/file_read_lang.h"
#include "diff_func/headers/bin_tree.h"
#include "headers/gen_asm.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = SRC_FILE;
    const char *output_file = DST_FILE;

    if (argc > 1) {
        input_file = argv[1];
        if (argc > 2)
            output_file = argv[2];
    }

    TreeStruct tree = {};

    if (ReadFileLang(&tree, input_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    if (GenGraphLang(&tree, GRAPH_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph gen failed\n");
        return ERROR;
    }

    if (GenAsmCode(&tree, output_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "asm code gen failed\n");
        return ERROR;
    }

    return SUCCESS;
}