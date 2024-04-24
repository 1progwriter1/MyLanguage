#include <stdio.h>
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "lex_analysis.h"
#include "parse.h"
#include "../graphviz/gen_graph_lang.h"
#include "prog_output.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = SRC_FILE;
    const char *output_file = DST_FILE;

    if (argc > 1) {
        input_file = argv[1];
        if (argc > 2)
            output_file = argv[2];
    }

    Vector data = {};
    Vector tokens = {};
    TreeStruct tree = {};

    if (analyzeLexis(&data, &tokens, input_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "lexical analysis failed\n");
        return ERROR;
    }

    if (stringParse(&tokens, &tree) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "string parse failed\n");
        return ERROR;
    }

    if (genGraphLang(&tree, GRAPH_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph generation failed\n");
        return ERROR;
    }

    if (printInFile(&tree, &data, output_file) != SUCCESS)
        return ERROR;

    TreeRootDtor(&tree);
    vectorDtor(&tokens);
    vectorDtor(&data);

    return SUCCESS;
}