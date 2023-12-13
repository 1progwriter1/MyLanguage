#include <stdio.h>
#include <assert.h>
#include "../headers/lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include <string.h>

// ask about const

#define KEY_WORD_LEN sizeof (KEY_WORDS[i]) - 1

#define IsFuncBegin (tokens->data[tokens->size - 1].type == FUNCTION)

static int ReadNumber(char **buf, double *num);
static int ReadKeyWord(char **buf, Vector *tokens, bool *is_found);
static int ReadName(char **buf, NamesTable *data, Vector *tokens);
static int ReadPunctuation(char **buf, Vector *tokens, bool *is_found);
static int ReadString(char **buf, NamesTable *data);
static bool IsNameExists(const char *str, NamesTable *data, size_t *index);

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

        if (*tmp == '"') {
            *tmp += 1;
            if (ReadString(&tmp, data) == SUCCESS)
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

        if (ReadName(&tmp, data, tokens) != SUCCESS)
            return ERROR;

    }

    if (PushBack(tokens, (Token) {PUNCT_SYM, {.code = END_SYMBOL}}) != SUCCESS)
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
        if (PushBack(tokens, (Token) {PUNCT_SYM, {.code = code}}) != SUCCESS)
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

            Token value = {};

            if (i < BINARY_OP_INDEX)
                value = {FUNCTION, {.func_index = 0}};
            else if (i < UNARY_OP_INDEX)
                value = {BINARY_OP, {.bin_op = (Binary_Op) (i - BINARY_OP_INDEX)}};
            else if (i < KEY_OP_INDEX)
                value = {UNARY_OP, {.un_op = (Unary_Op) (i - UNARY_OP_INDEX)}};
            else if (i < ADD_WORDS_INDEX)
                value = {KEY_OP, {.key_op = (Key_Op) (i - KEY_OP_INDEX)}};

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

static int ReadName(char **buf, NamesTable *data, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(data);
    assert(tokens);

    size_t len_of_var = 0;
    while (*buf[len_of_var] != ' ')
        len_of_var++;

    char *name = (char *) calloc (len_of_var + 1, sizeof (char));
    if (!name) return NO_MEMORY;

    sscanf(*buf, "%s", name);

    NameType type = IsFuncBegin ? FUNC_NAME : VAR_NAME;

    size_t name_index = 0;
    if (IsNameExists(name, data, &name_index)) {

    }
    if (type == FUNC_NAME)
        tokens->data[tokens->size - 1].func_index = data->size;

    if (PushName(data, (Name) {name, type}) != SUCCESS) {
        printf(RED "error: " END_OF_COLOR "unable to add new variable\n");
        return ERROR;
    }

    *buf += len_of_var;

    return SUCCESS;
}

static int ReadString(char **buf, NamesTable *data) {

    assert(buf);
    assert(*buf);
    assert(data);

    size_t len_of_str = 0;
    while (*buf[len_of_str] != ' ')
        len_of_str++;

    char *string = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!string) return NO_MEMORY;

    sscanf(*buf, "%[^\"]", string);

    if (PushName(data, (Name) {string, STRING}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static bool IsNameExists(const char *str, NamesTable *data) {

    assert(str);
    assert(data);
    assert(index);

    for (size_t i = NUMBER_OF_KEY_WORDS - 1; i < data->size; i++)
        if (strcmp(data->names[i].name, str) == 0) {
            *index = i;
            return true;
        }

    return false;
}