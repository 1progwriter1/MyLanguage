#include <stdio.h>
#include "../MyLibraries/headers/systemdata.h"
#include "Graphviz/gen_graph_lang.h"
#include "headers/file_read_lang.h"
#include "diff_func/headers/bin_tree.h"
#include "headers/gen_asm.h"

int main() {

    TreeStruct tree = {};

    if (ReadFileLang(&tree, PROG_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    if (GenGraphLang(&tree, GRAPH_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph gen failed\n");
        return ERROR;
    }

    if (GenAsmCode(&tree, ASM_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "asm code gen  failed\n");
        return ERROR;
    }

    return SUCCESS;
}