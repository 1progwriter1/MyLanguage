#include <stdio.h>
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "lex_analysis.h"
#include "parse.h"
#include "../graphviz/gen_graph_lang.h"
#include "../lib_src/my_lan_lib.h"
#include "prog_output.h"
#include "dump.h"
#include <string.h>
#include <stdlib.h>

#define DTOR_DATA namesDtor(&names_table);  treeRootDtor(&tree);  vectorDtor(&tokens);    vectorDtor(&names_table); free(output_file);
#define INPUT_FORMAT ".fly"
#define OUTPUT_FORMAT ".mo"

char *createOutputFile(const char *filename);

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *input_file = NULL;
    char *output_file = NULL;

    if (argc > 1) {
        input_file = argv[1];
        output_file = createOutputFile(input_file);
        if (!output_file)   return ERROR;
    }
    else {
        printf(RED "error: " END_OF_COLOR "file expected\n");
        return ERROR;
    }

    Vector names_table = {};
    Vector tokens = {};
    TreeStruct tree = {};

    if (analyzeLexis(&names_table, &tokens, input_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "lexical analysis failed\n");
        DTOR_DATA
        return ERROR;
    }

    dumpAll(&tokens, &names_table, "dump.txt");

    if (stringParse(&tokens, &tree, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "string parse failed\n");
        DTOR_DATA
        return ERROR;
    }

    if (genGraphLang(&tree, GRAPH_FILE, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph generation failed\n");
        DTOR_DATA
        return ERROR;
    }

    if (printInFile(&tree, &names_table, output_file) != SUCCESS) {
        DTOR_DATA
        return ERROR;
    }

    DTOR_DATA

    return SUCCESS;
}

char *createOutputFile(const char *filename) {

    assert(filename);

    char *ptr = const_cast<char *>(filename);
    char *format = strstr(ptr, INPUT_FORMAT);
    if (!format) {
        printf(RED "error: " END_OF_COLOR "wrong file format\n");
        return NULL;
    }
    char *output = (char *) calloc ((size_t) (format - filename) + 1 + sizeof(OUTPUT_FORMAT), sizeof(char));
    if (!output) return NULL;

    strncpy(output, filename, (size_t) (format - filename));
    strcat(output, OUTPUT_FORMAT);

    return output;
}