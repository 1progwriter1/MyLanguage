#include <stdio.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../backend_intel/file_read_lang.h"
#include <assert.h>
#include "../headers/derivative.h"
#include "../graphviz/gen_graph_lang.h"
#include "../frontend_src/prog_output.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *src_file = SRC_FILE;

    if (argc > 1)
        src_file = argv[1];

    TreeStruct tree = {};

    if (ReadFileLang(&tree, src_file) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    if (SimplifyTree(&tree) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "optimization failed\n");
        return ERROR;
    }

    if (GenGraphLang(&tree, GRAPH_FILE) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "graphviz failed\n");
        return ERROR;
    }

    if (PrintInFile(&tree, src_file) != SUCCESS) {
        printf(RED "middle end error: " END_OF_COLOR "print in file failed\n");
        return ERROR;
    }


    return SUCCESS;
}