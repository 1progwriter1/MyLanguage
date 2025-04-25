#include <stdio.h>
#include "systemdata.h"
#include "file_read_lang.h"
#include <assert.h>
#include "derivative.h"
#include "../graphviz/gen_graph_lang.h"
#include "prog_output.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *src_file = NULL;

    if (argc > 1)
        src_file = argv[1];
    else
        return ERROR;

    TreeStruct tree = {};
    Vector names_table = {};

    if (readFileLang(&tree, src_file, &names_table) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    if (simplifyTree(&tree) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "optimization failed\n");
        return ERROR;
    }

    if (genGraphLang(&tree, GRAPH_FILE, &names_table) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "graphviz failed\n");
        return ERROR;
    }

    if (printInFile(&tree, &names_table, src_file) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "print in file failed\n");
        return ERROR;
    }


    return SUCCESS;
}
