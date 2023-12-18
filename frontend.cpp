#include <stdio.h>
#include "../MyLibraries/headers/systemdata.h"
#include "headers/names_table.h"
#include "headers/lex_analysis.h"
#include "headers/parse.h"
#include "Graphviz/gen_graph_lang.h"
#include "headers/prog_output.h"

int main() {

    NamesTable data = {};
    Vector tokens = {};
    TreeStruct tree = {};

    if (LexicalAnalysis(&data, &tokens, "text.txt") != SUCCESS) {
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

    if (PrintInFile(&tree, PROG_FILE) != SUCCESS)
        return ERROR;

    return SUCCESS;
}