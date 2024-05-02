#include <stdio.h>
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../graphviz/gen_graph_lang.h"
#include "file_read_lang.h"
#include "../bin_tree/bin_tree.h"
#include "gen_asm.h"
#include "../frontend_src/lex_analysis.h"
#include <string.h>
#include <stdlib.h>

#define DTOR_DATA namesBackendDtor(&names_table);  treeRootDtor(&tree);  vectorDtor(&names_table); free(output_file);
#define INPUT_FORMAT ".mo"
#define OUTPUT_FORMAT ".mout"

char *createOutputFile(const char *filename);
void namesBackendDtor(Vector *names_table);

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

    TreeStruct tree = {};
    Vector names_table = {};
    if (vectorCtor(&names_table, 8, sizeof(Name)) != SUCCESS)
        return ERROR;

    if (readFileLang(&tree, input_file, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "file read failed\n");
        DTOR_DATA
        return ERROR;
    }

    for (size_t i = 0; i < names_table.size; i++) {
        printf("%lu) %s\n", i, ((Name *) getPtr(&names_table, i))->name);
    }

    if (genGraphLang(&tree, GRAPH_FILE, &names_table) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "graph gen failed\n");
        DTOR_DATA
        return ERROR;
    }

    if (genAsmCode(&tree, output_file) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "asm code gen failed\n");
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

void namesBackendDtor(Vector *names_table) {

    assert(names_table);

    for (size_t i = 0; i < names_table->size; i++) {
        char *ptr = const_cast<char *>(((Name *) getPtr(names_table, i))->name);
        free(ptr);
        ((Name *) getPtr(names_table, i))->name = NULL;
    }
}