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

#define KEY_WORD_LEN strlen(KEY_WORDS[i])

#define IfFuncBegins (tokens->data[tokens->size - 1].type == FUNCTION)


static ReadStatus ReadNumber(char **buf, Vector *tokens);
static ReadStatus ReadKeyWord(char **buf, Vector *tokens);
static ReadStatus ReadName(char **buf, NamesTable *data, Vector *tokens);
static ReadStatus ReadPunctuation(char **buf, Vector *tokens);
static ReadStatus ReadString(char **buf, Vector *tokens);
static int PrototypesAnalysis(char **buf, NamesTable *data);
static bool IfNameExists(const char *str, NamesTable *data, size_t *index);
static bool IsValidSymbol(const char sym);
static void SkipSpaces(char **buf);
static void SkipComments(char **buf);
static char *GetName(char **buf);


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
    if (PrototypesAnalysis(&tmp, data) != SUCCESS)
        return ERROR;

    while (*tmp != '\0') {

        SkipSpaces(&tmp);
        SkipComments(&tmp);
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

        status = ReadNumber(&tmp, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;

        status = ReadKeyWord(&tmp, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;;

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

    char *name = GetName(buf);

    NameType type = IfFuncBegins ? FUNC_NAME : VAR_NAME;

    size_t name_index = 0;
    if (!IfNameExists(name, data, &name_index)) {
        name_index = data->size;

        if (PushName(data, {name, type}) != SUCCESS)
            return kReadStatusNoMemory;
    }
    else {
        free(name);
        name = NULL;
    }

    if (type == FUNC_NAME) {
        tokens->data[tokens->size - 1].func_index = name_index;
    }
    else if (data->names[name_index].type == FUNC_NAME) {
            if (PushBack(tokens, {FUNCTION ,{.func_index = name_index}}) != SUCCESS)
                return kReadStatusNoMemory;
    }
    else {
        if (PushBack(tokens, {VARIABLE, {.var_index = name_index}}) != SUCCESS)
            return kReadStatusNoMemory;
    }

    return kReadStatusFound;
}


static ReadStatus ReadString(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    size_t len_of_str = 0;
    while ((*buf)[len_of_str] != '\"' && (*buf)[len_of_str] != '\0')
        len_of_str++;

    char *string = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!string) return kReadStatusNoMemory;

    sscanf(*buf, "%[^\"]", string);

    if (PushBack(tokens, {STRING, {.string = string}}) != SUCCESS)
        return kReadStatusNoMemory;

    *buf += len_of_str + 1;

    return kReadStatusFound;
}

static int PrototypesAnalysis(char **buf, NamesTable *data) {

    assert(buf);
    assert(*buf);
    assert(data);

    while (true) {
        SkipSpaces(buf);
        SkipComments(buf);
        SkipSpaces(buf);

        if (strncmp(*buf, "once upon the time", sizeof ("once upon the time") - 1) == 0)
            return SUCCESS;

        if (strncmp(*buf, "fairytale", sizeof ("fairytale") - 1) == 0) {
            *buf += sizeof ("fairytale") - 1;
            SkipSpaces(buf);
            char *func_name = GetName(buf);
            if (!func_name)
                return ERROR;

            if (PushName(data, {func_name, FUNC_NAME}) != SUCCESS)
                return ERROR;
        }

        *buf += 1;
    }
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

    const size_t PUNC_SYM_NUM = 14;
    char punct_symbols[PUNC_SYM_NUM] = {'(', ')', '{', '}', ';', '\n', ' ', '"', '*', '\\', '-', '+', '^', ' '};

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

static void SkipComments(char **buf) {

    assert(buf);
    assert(*buf);

    if (**buf == '/' && *(*buf + 1) == '/') {
        *buf += 2;
        while (**buf != '\n')
            *buf += 1;
        *buf += 1;
    }
}

static char *GetName(char **buf) {

    assert(buf);
    assert(*buf);

    size_t len_of_str = 0;
    while (IsValidSymbol((*buf)[len_of_str]))
        len_of_str++;

    if (len_of_str == 0) {
        printf(RED "error: " END_OF_COLOR "invalid name \"%s\"\n", *buf);
        return NULL;
    }

    char *str = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!str) return NULL;

    sscanf(*buf, "%[^(){} \n\";-+*\\^]", str);

    *buf += len_of_str;

    return str;
}