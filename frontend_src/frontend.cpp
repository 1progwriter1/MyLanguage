#include <stdio.h>
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "lex_analysis.h"
#include "parse.h"
#include "../graphviz/gen_graph_lang.h"
#include "prog_output.h"
#include "dump.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = SRC_FILE;
    const char *output_file = DST_FILE;

    if (argc > 1) {
        input_file = argv[1];
        if (argc > 2)
            output_file = argv[2];
    }

    Vector names_table = {};
    Vector tokens = {};
    TreeStruct tree = {};

    if (analyzeLexis(&names_table, &tokens, input_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "lexical analysis failed\n");
        return ERROR;
    }

    dumpAll(&tokens, &names_table, "dump.txt");

    if (stringParse(&tokens, &tree, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "string parse failed\n");
        return ERROR;
    }

    if (genGraphLang(&tree, GRAPH_FILE, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph generation failed\n");
        return ERROR;
    }

    if (printInFile(&tree, &names_table, output_file) != SUCCESS)
        return ERROR;

    treeRootDtor(&tree);
    vectorDtor(&tokens);
    vectorDtor(&names_table);

    return SUCCESS;
}