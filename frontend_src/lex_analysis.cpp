#include <stdio.h>
#include <assert.h>
#include "../headers/lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include <string.h>

const char *KEY_WORDS[NUMBER_OF_KEY_WORDS] = {  "Once upon a time",                         // begin main
                                                "and they lived happily ever after",        // end main
                                                "fairytale character",                      // double
                                                "stone", "go left", "go right",             // if, then, else
                                                "fell into a dead sleep while",             // while
                                                "fairytale",                                // begin function
                                                "end",                                      // end function
                                                "did not turn into", "turned into",         // !=, ==
                                                "turn into", "stronger", "weeker",          // =, >, <
                                                "not stronger", "not weeker",               // <=, >=
                                                "add", "sub", "mul", "div", "pow", "not",   // +, -, *, /, ^
                                                "guess the riddle", "say the magic word"    // , printf
                                                };

static int ReadNumber(char **buf, double *num);
static int ReadSystemWord(char **buf, SWCodes *code);
static int ReadVariable(char **buf, NamesTable *data);

const size_t START_NUM_OF_TOKENS = 8;

int LexicalAnalysis(NamesTable *data, Vector *tokens, const char *filename) {

    assert(filename);
    assert(tokens);
    assert(data);

    char *buf = readbuf(filename);
    if (!buf) return ERROR;

    if (NamesTableCtor(data, KEY_WORDS) != SUCCESS) {
        free(buf);
        return ERROR;
    }

    if (VectorCtor(tokens, START_NUM_OF_TOKENS) != SUCCESS) {
        free(buf);
        NamesTableDtor(data);
        return ERROR;
    }

    char *tmp = buf;
    while (*tmp != '\0') {

        if (*tmp == ' ') {
            tmp++;
            continue;
        }

        char *

    }

    if (PushBack(tokens, (Token) {TOKEN_END_SYM, {}}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}
//const
static int ReadNumber(char **buf, double *num) {

    assert(buf);
    assert(*buf);
    assert(num);

    char *end = NULL;
    *num = strtod(*buf, &end);

    if (end == *buf) return ERROR;

    *buf = end;

    return SUCCESS;
}

static int ReadSystemWord(char **buf, SWCodes *code) {

    assert(buf);
    assert(*buf);
    assert(code);

    for (size_t i = 0; i < sizeof (SYSTEM_WORDS) / sizeof (SYSTEM_WORDS[0]); i++) {
        if (strncmp(*buf, SYSTEM_WORDS[i], sizeof (SYSTEM_WORDS[i]) - 1) == 0) {
            *code = (SWCodes) i;
            *buf += sizeof (SYSTEM_WORDS[i]) - 1;
            return SUCCESS;
        }
    }

    return ERROR;
}

static int ReadVariable(char **buf, NamesTable *data) {

    assert(buf);
    assert(*buf);
    assert(data);

    size_t len_of_var = 0;
    while (*buf[len_of_var] != ' ')
        len_of_var++;

    char *name = (char *) calloc (len_of_var, sizeof (char));
    if (!name) return NO_MEMORY;

    sscanf(*buf, "%s", name);

    if (PushName(data, (Name) {name, 0}) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "unable to add new variable\n");
        return ERROR;
    }

    *buf += len_of_var;

    return SUCCESS;
}