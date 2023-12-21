#include <stdio.h>
#include <assert.h>
#include "../MyLibraries/headers/systemdata.h"
#include "headers/names_table.h"
#include "headers/lex_analysis.h"
#include "headers/parse.h"
#include "Graphviz/gen_graph_lang.h"
#include "headers/prog_output.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = SRC_FILE;
    const char *output_file = DST_FILE;

    if (argc > 1) {
        input_file = argv[1];
        if (argc > 2)
            output_file = argv[2];
    }

    NamesTable data = {};
    Vector tokens = {};
    TreeStruct tree = {};

    if (LexicalAnalysis(&data, &tokens, input_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "lexical analysis failed\n");
        return ERROR;
    }

    if (StringParse(&tokens, &tree) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "string parse failed\n");
        return ERROR;
    }

    if (GenGraphLang(&tree, GRAPH_FILE) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph generation failed\n");
        return ERROR;
    }

    if (PrintInFile(&tree, output_file) != SUCCESS)
        return ERROR;

    TreeRootDtor(&tree);
    VectorDtor(&tokens);

    return SUCCESS;
}