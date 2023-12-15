#include <stdio.h>
#include "../MyLibraries/headers/systemdata.h"
#include "headers/names_table.h"
#include "headers/lex_analysis.h"
#include "headers/parse.h"

// bash script

int main() {
    NamesTable data = {};
    Vector tokens = {};

    if (LexicalAnalysis(&data, &tokens, "text.txt") != SUCCESS)
        return ERROR;

    for (size_t i = 0; i < tokens.size; i++)
        printf("%lu) %d\n", i, tokens.data[i]);

    printf("\n\n");

    for (size_t i = 0; i < data.size; i++)
        printf("\"%s\" [%d]\n", data.names[i].name, data.names[i].type);

    return SUCCESS;
}