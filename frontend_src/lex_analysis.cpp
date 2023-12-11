#include <stdio.h>
#include <assert.h>
#include "../headers/lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include <errno.h>
#include <string.h>

const char *KEY_WORDS[] = {  "Once upon the time",                          // begin main
                                "and they lived happily ever after",        // end main
                                "king", "queen", "prince", "princess"       // double
                                "stone", "go left", "go right",             // if, then, else
                                "fell into a dead sleep while"              // while
                                "fairy tale",                               // begin function
                                "end"                                       // end function
                                "did not turn into", "turned into"          // !=, ==
                                "turn into", "stronger", "weeker",          // =, >, <
                                "not stronger", "not weeker"                // <=, >=
                                "add", "sub", "mul", "div", "pow", "not"    // +, -, *, /, ^
                            };

static int ReadNumber(char **buf, double *num);
static int ReadSystemWord(char **buf, SWCodes *code);
static int ReadVariable(char **buf, NamesTable *data);

int LexicalAnalysis(NamesTable *data, Vector *tokens, char *buf) {

    assert(buf);
    assert(tokens);
    assert(data);

    char *tmp = buf;
    while (*tmp != '\0') {

        if (*tmp == ' ') {
            tmp++;
            continue;
        }

        switch (*tmp) {
            case ('+'): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OPERATION, {.operation = TOKEN_ADD}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case ('-'): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OPERATION, {.operation = TOKEN_SUB}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case ('*'): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OPERATION, {.operation = TOKEN_MUL}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case ('/'): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OPERATION, {.operation = TOKEN_DIV}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case ('('): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OP_BRACKET, {}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case (')'): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_CL_BRACKET, {}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            case ('='): {
                tmp++;
                if (PushBack(tokens, (Token) {TOKEN_OPERATION, {.operation = TOKEN_ASSIGN}}) != SUCCESS)
                    return ERROR;
                continue;
            }
            default: {
                break;
            }
        }

        SWCodes code = SW_COS;
        if (ReadSystemWord(&tmp, &code) == SUCCESS) {
            if (PushBack(tokens, (Token) {TOKEN_SYS_WORD, {.sys_word = code}}) != SUCCESS)
                return ERROR;
            continue;
        }

        double num = 0;
        if (ReadNumber(&tmp, &num) == SUCCESS) {
            if (PushBack(tokens, (Token) {TOKEN_NUMBER, {.number = num}}) != SUCCESS)
                return ERROR;
            continue;
        }

        if (ReadVariable(&buf, data) != SUCCESS)
            return ERROR;

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