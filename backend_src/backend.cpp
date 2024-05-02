#include <stdio.h>
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../graphviz/gen_graph_lang.h"
#include "file_read_lang.h"
#include "../bin_tree/bin_tree.h"
#include "gen_asm.h"
#include "../frontend_src/lex_analysis.h"
#include <string.h>

#define FILE_FORMAT ".ml"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = BACK_SRC_FILE;
    const char *output_file = DST_FILE;

    if (argc > 1) {
        input_file = argv[1];
        if (argc > 2)
            output_file = argv[2];
    }

    TreeStruct tree = {};
    Vector names_table = {};
    if (vectorCtor(&names_table, 8, sizeof(Name)) != SUCCESS)
        return ERROR;

    if (readFileLang(&tree, input_file, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    if (genGraphLang(&tree, GRAPH_FILE, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph gen failed\n");
        return ERROR;
    }

    if (genAsmCode(&tree, output_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "asm code gen failed\n");
        return ERROR;
    }

    return SUCCESS;
}