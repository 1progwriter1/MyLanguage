#include <stdio.h>
#include <assert.h>
#include "../headers/lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include <string.h>

// ask about const

#define KEY_WORD_LEN sizeof (KEY_WORDS[i]) - 1

static const char *KEY_WORDS[NUMBER_OF_KEY_WORDS] = {   "Once upon a time",                         // begin main
                                                        "and they lived happily ever after",        // end main
                                                        "fairytale character",                      // double
                                                        "stone", "go left", "go right",             // if, then, else
                                                        "fell into a dead sleep while",             // while
                                                        "fairytale",                                // begin function
                                                        "end",                                      // end function
                                                        "guess the riddle", "say the magic word",   // , printf
                                                        "sin", "cos", "sqrt", "ln", "not",          // sin, cos, sqrt, ln, not
                                                        "did not turn into", "turned into",         // !=, ==
                                                        "turn into", "stronger", "weeker",          // =, >, <
                                                        "not stronger", "not weeker",               // <=, >=
                                                        "+", "-", "*", "/", "^"                     // +, -, *, /, ^
                                                    };

static int ReadNumber(char **buf, double *num);
static int ReadKeyWord(char **buf, Vector *tokens, bool *is_found);
static int ReadVariable(char **buf, NamesTable *data);
static int ReadPunctuation(char **buf, Vector *tokens, bool *is_found);

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

        bool is_found = false;
        if (ReadPunctuation(&tmp, tokens, &is_found) == SUCCESS) {
            continue;
        }
        else if (is_found) {
            return ERROR;
        }

        if (ReadKeyWord(&tmp, tokens, &is_found) == SUCCESS) {
            continue;
        }
        else if (is_found) {
            return ERROR;
        }

        double num = 0;
        if (ReadNumber(&tmp, &num) == SUCCESS)
            continue;

        if (ReadVariable(&tmp, data) != SUCCESS)
            return ERROR;

    }

    if (PushBack(tokens, (Token) {TOKEN_PUNCT_SYM, {.code = END_SYMBOL}}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int ReadPunctuation(char **buf, Vector *tokens, bool *is_found) {

    assert(buf);
    assert(tokens);
    assert(is_found);

    Punctuation code = END_SYMBOL;
    *is_found = true;

    switch (**buf) {
        case ('\0'): {
            code = END_SYMBOL;
            break;
        }
        case ('('): {
            code = OP_PARENTHESIS;
            break;
        }
        case (')'): {
            code = CL_PARENTHESIS;
            break;
        }
        case ('{'): {
            code = OP_BRACE;
            break;
        }
        case ('}'): {
            code = CL_PARENTHESIS;
            break;
        }
        case (';'): {
            code = NEW_LINE;
            break;
        }
        default: {
            *is_found = false;
            break;
        }
    }

    if (*is_found) {
        *buf += 1;
        if (PushBack(tokens, (Token) {TOKEN_PUNCT_SYM, {.code = code}}) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }

    return ERROR;
}

static int ReadKeyWord(char **buf, Vector *tokens, bool *is_found) {

    assert(buf);
    assert(*buf);
    assert(tokens);
    assert(index);

    *is_found = false;
    for (size_t i = 0; i < NUMBER_OF_KEY_WORDS; i++) {
        if (strncmp(*buf, KEY_WORDS[i], KEY_WORD_LEN) == 0) {
            *is_found = true;

            if (PushBack(tokens, (Token) {TOKEN_KEY_WORD, {.key_word = i}}) != SUCCESS)
                return ERROR;

            *buf += KEY_WORD_LEN;
            return SUCCESS;
        }
    }

    return ERROR;
}

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

    if (PushName(data, (Name) {name, VAR_NAME}) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "unable to add new variable\n");
        return ERROR;
    }

    *buf += len_of_var;

    return SUCCESS;
}