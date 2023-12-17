#include <stdio.h>
#include <assert.h>
#include "../headers/lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include <string.h>
#include "../headers/lang_syntax.h"

enum ReadStatus
{
    kReadStatusFound,
    kReadStatusNoMemory,
    kReadStatusNotFound,
};

// ask about const

#define KEY_WORD_LEN strlen(KEY_WORDS[i])

#define IfFuncBegins (tokens->data[tokens->size - 1].type == FUNCTION)


static ReadStatus ReadNumber(char **buf, Vector *tokens);
static ReadStatus ReadKeyWord(char **buf, Vector *tokens);
static ReadStatus ReadName(char **buf, NamesTable *data, Vector *tokens);
static ReadStatus ReadPunctuation(char **buf, Vector *tokens);
static ReadStatus ReadString(char **buf, Vector *tokens);
static bool IfNameExists(const char *str, NamesTable *data, size_t *index);
static bool IsValidSymbol(const char sym);
static void SkipSpaces(char **buf);


int LexicalAnalysis(NamesTable *data, Vector *tokens, const char *filename) {

    assert(filename);
    assert(tokens);
    assert(data);

    char *buf = readbuf(filename);
    if (!buf) return ERROR;

    if (NamesTableCtor(data) != SUCCESS) {
        free(buf);
        return ERROR;
    }

    const size_t INITIAL_NUM_OF_TOKENS = 8;
    if (VectorCtor(tokens, INITIAL_NUM_OF_TOKENS) != SUCCESS) {
        free(buf);
        NamesTableDtor(data);
        return ERROR;
    }

    char *tmp = buf;
    while (*tmp != '\0') {

        SkipSpaces(&tmp);

        ReadStatus status = kReadStatusFound;
        if (*tmp == '"') {
            tmp++;

            status = ReadString(&tmp, tokens);
            if (status == kReadStatusNoMemory) return NO_MEMORY;
            else if (status == kReadStatusFound) continue;
        }

        status = ReadPunctuation(&tmp, tokens);
        if (status == kReadStatusFound) continue;
        else if (status == kReadStatusNoMemory) return NO_MEMORY;

        status = ReadKeyWord(&tmp, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;;

        status = ReadNumber(&tmp, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;

        status = ReadName(&tmp, data, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusNotFound) return ERROR;
    }

    if (PushBack(tokens, {PUNCT_SYM, {.sym_code = END_SYMBOL}}) != SUCCESS)
        return ERROR;

    return SUCCESS;
}


static ReadStatus ReadPunctuation(char **buf, Vector *tokens) {

    assert(buf);
    assert(tokens);

    Punctuation code = END_SYMBOL;

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
            code = CL_BRACE;
            break;
        }
        case (';'): {
            code = NEW_LINE;
            break;
        }
        default: {
            return kReadStatusNotFound;
        }
    }

    *buf += 1;
    if (PushBack(tokens, {PUNCT_SYM, {.sym_code = code}}) != SUCCESS)
        return kReadStatusNoMemory;

    return kReadStatusFound;
}

static ReadStatus ReadKeyWord(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    for (size_t i = 0; i < NUMBER_OF_KEY_WORDS; i++) {
        if (strncmp(*buf, KEY_WORDS[i], KEY_WORD_LEN) == 0) {

            *buf += KEY_WORD_LEN;
            Token value = {};

            if (i < BINARY_OP_INDEX)
                value = {UNARY_OP, {.un_op = (Unary_Op) i}};

            else if (i < KEY_OP_INDEX)
                value = {BINARY_OP, {.bin_op = (Binary_Op) i}};

            else if (i < FUNCTION_DEF_INDEX)
                value = {KEY_OP, {.key_op = (Key_Op) i}};

            else if (i < ADD_WORDS_INDEX)
                value = {FUNCTION, {.func_index = 0}};
            else
                return kReadStatusFound;

            if (PushBack(tokens, value) != SUCCESS)
                return kReadStatusNoMemory;

            return kReadStatusFound;
        }
    }

    return kReadStatusNotFound;
}


static ReadStatus ReadNumber(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    char *end = NULL;
    double num = strtod(*buf, &end);

    if (end == *buf) return kReadStatusNotFound;

    if (PushBack(tokens, {NUMBER, {.number = num}}) != SUCCESS)
        return kReadStatusNoMemory;

    *buf = end;

    return kReadStatusFound;
}


static ReadStatus ReadName(char **buf, NamesTable *data, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(data);
    assert(tokens);

    size_t len_of_str = 0;
    while (IsValidSymbol((*buf)[len_of_str]))
        len_of_str++;

    if (len_of_str == 0) {
        printf(RED "error: " END_OF_COLOR "invalid variable name \"%s\"\n", *buf);
        return kReadStatusNotFound;
    }

    char *name = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!name) return kReadStatusNoMemory;

    sscanf(*buf, "%[^(){} \n\";]", name);

    NameType type = IfFuncBegins ? FUNC_NAME : VAR_NAME;

    size_t name_index = 0;
    if (!IfNameExists(name, data, &name_index)) {
        name_index = data->size;

        if (PushName(data, {name, type}) != SUCCESS) {
            printf(RED "error: " END_OF_COLOR "unable to add new variable\n");
            return kReadStatusNoMemory;
        }
    }

    if (type == FUNC_NAME)
        tokens->data[tokens->size - 1].func_index = name_index;
    else
        if (PushBack(tokens, {VARIABLE, {.var_index = name_index}}) != SUCCESS)
            return kReadStatusNoMemory;

    *buf += len_of_str;

    return kReadStatusFound;
}


static ReadStatus ReadString(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    size_t len_of_str = 0;
    while ((*buf)[len_of_str] != '\"')
        len_of_str++;

    char *string = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!string) return kReadStatusNoMemory;

    sscanf(*buf, "%[^\"]", string);

    if (PushBack(tokens, {STRING, {.string = string}}) != SUCCESS)
        return kReadStatusNoMemory;

    *buf += len_of_str + 1;

    return kReadStatusFound;
}


static bool IfNameExists(const char *str, NamesTable *data, size_t *name_index) {

    assert(str);
    assert(data);
    assert(name_index);

    for (size_t i = NUMBER_OF_KEY_WORDS - 1; i < data->size; i++)
        if (strcmp(data->names[i].name, str) == 0) {
            *name_index = i;
            return true;
        }

    return false;
}


static bool IsValidSymbol(const char sym) {

    const size_t PUNC_SYM_NUM = 8;
    char punct_symbols[PUNC_SYM_NUM] = {'(', ')', '{', '}', ';', '\n', ' ', '"'};

    for (size_t i = 0; i < PUNC_SYM_NUM; i++)
        if (sym == punct_symbols[i])
            return false;

    return true;
}

static void SkipSpaces(char **buf) {

    assert(buf);
    assert(*buf);

    while (**buf == ' ' || **buf == '\n')
        *buf += 1;
}