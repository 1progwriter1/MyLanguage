#include <stdio.h>
#include <assert.h>
#include "lex_analysis.h"
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include <string.h>

enum ReadStatus {
    kReadStatusFound,
    kReadStatusNoMemory,
    kReadStatusNotFound,
};

#define KEY_WORD_LEN strlen(((Name *) getPtr(names_table, i))->name)
#define IfFuncBegins (tokens->data[tokens->size - 1].type == FUNCTION)

#define BEGIN_OF_MAIN "once upon the time"
#define BEGIN_OF_FUNCTIONS "fairytale"

static int prepareToAnalyze(Vector *names_table, Vector *tokens);

static int prototypesAnalysis(char **buf, Vector *data);

static ReadStatus readNumber     (char **buf, Vector *tokens);
static ReadStatus readString     (char **buf, Vector *tokens);
static ReadStatus readKeyWord    (char **buf, Vector *tokens, Vector *names_table);
static ReadStatus readPunctuation(char **buf, Vector *tokens);
static ReadStatus readName       (char **buf, Vector *tokens, Vector *names_table);

// static int readName(char **buf, NameType type, Vector *tokens);

static bool IfNameExists(const char *str, Vector *names_table, int *index);
static bool IfFunctionExists(const char *str, Vector *names_table, int *func_index);
static bool IsValidSymbol(const char sym);
static void skipSpaces(char **buf);
static void skipComments(char **buf);

static Name *getName(char **buf, NameType type);
void nameDtor(Name *name);
void *getPtr(Vector *data, size_t index);


int analyzeLexis(Vector *names_table, Vector *tokens, const char *filename) {

    assert(filename);
    assert(tokens);
    assert(names_table);

    char *buf = readFileToBuffer(filename);
    if (!buf) return ERROR;

    if (prepareToAnalyze(names_table, tokens) != SUCCESS)
        return ERROR;

    char *tmp = buf;
    if (prototypesAnalysis(&tmp, names_table) != SUCCESS)
        return ERROR;

    while (*tmp != '\0') {

        skipSpaces(&tmp);
        skipComments(&tmp);
        skipSpaces(&tmp);

        ReadStatus status = kReadStatusFound;
        if (*tmp == '"') {
            tmp++;
            status = readString(&tmp, tokens);
            if (status == kReadStatusNoMemory) return NO_MEMORY;
            else if (status == kReadStatusFound) continue;
        }

        status = readPunctuation(&tmp, tokens);
        if (status == kReadStatusFound) continue;
        else if (status == kReadStatusNoMemory) return NO_MEMORY;

        status = readNumber(&tmp, tokens);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;

        status = readKeyWord(&tmp, tokens, names_table);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusFound) continue;;

        status = readName(&tmp, tokens, names_table);
        if (status == kReadStatusNoMemory) return NO_MEMORY;
        else if (status == kReadStatusNotFound) return ERROR;
    }

    if (pushBack(tokens, {PUNCT_SYM, NULL) != SUCCESS)
        return ERROR;

    free(buf);

    return SUCCESS;
}

static int prepareToAnalyze(Vector *names_table, Vector *tokens) {

    assert(names_table);
    assert(tokens);

    if (vectorCtor(names_table, NUMBER_OF_KEY_WORDS, sizeof(Token)) != SUCCESS)
        return ERROR;

    if (vectorCtor(tokens, 8, sizeof(Token)) != SUCCESS)
        return ERROR;

    Name *tmp = (Name *) calloc (1, sizeof(Name));
    if (!tmp) {
        printf(RED "LA error: " END_OF_COLOR "failed to allocate memory during preparation\n");
        return ERROR;
    }

    for (size_t i = 0; i < NUMBER_OF_KEY_WORDS; i++) {
        tmp->name = KEY_WORDS[i];
        tmp->type = KEY_WORD;
        if (pushBack(names_table, tmp) != SUCCESS)
            return ERROR;
    }

    free(tmp);

    return SUCCESS;
}

static int prototypesAnalysis(char **buf, Vector *names_table) {

    assert(buf);
    assert(*buf);
    assert(names_table);

    while (true) {

        skipSpaces(buf);
        skipComments(buf);
        skipSpaces(buf);

        if (strncmp(*buf, BEGIN_OF_MAIN, sizeof(BEGIN_OF_MAIN) - 1) == 0)
            return SUCCESS;

        if (strncmp(*buf, BEGIN_OF_FUNCTIONS, sizeof (BEGIN_OF_FUNCTIONS) - 1) == 0) {
            *buf += sizeof (BEGIN_OF_FUNCTIONS) - 1;
            skipSpaces(buf);

            if (readName(buf, FUNC_NAME, names_table) != SUCCESS)
                return ERROR;

            while (**buf != ';' && **buf != '\0')
                *buf += 1;
            *buf += 1;
        }

        *buf += 1;
    }
}


static ReadStatus readPunctuation(char **buf, Vector *tokens) {

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
        case (','): {
            code = COMMA;
            break;
        }
        default: {
            return kReadStatusNotFound;
        }
    }

    Token *tmp = (Token *) calloc (1, sizeof(Token));
    if (!tmp)   return kReadStatusNoMemory;

    tmp->type = PUNCT_SYM;
    tmp->sym_code = code;

    *buf += 1;

    if (pushBack(tokens, tmp) != SUCCESS)
        return kReadStatusNoMemory;

    return kReadStatusFound;
}

static ReadStatus readKeyWord(char **buf, Vector *tokens, Vector *names_table) {

    assert(buf);
    assert(*buf);
    assert(tokens);
    assert(names_table);

    for (size_t i = 0; i < NUMBER_OF_KEY_WORDS; i++) {
        if (strncmp(*buf, ((Name *) getPtr(names_table, i))->name, KEY_WORD_LEN) == 0) {

            *buf += KEY_WORD_LEN;
            Token *value = (Token *) calloc (1, sizeof(Token));
            if (!value) return kReadStatusNoMemory;

            if (i < BINARY_OP_INDEX) {
                value->type  = UNARY_OP;
                value->un_op = (Unary_Op) i;
            }
            else if (i < KEY_OP_INDEX) {
                value->type = BINARY_OP;
                value->bin_op = (Binary_Op) i;
            }

            else if (i < FUNCTION_DEF_INDEX) {
                value->type = KEY_OP;
                value->key_op = (Key_Op) i;
            }
            else if (i < ADD_WORDS_INDEX) {
                value->type = FUNCTION;
                value->func_index = 0;
            }
            else
                return kReadStatusFound;

            if (pushBack(tokens, value) != SUCCESS)
                return kReadStatusNoMemory;

            return kReadStatusFound;
        }
    }

    return kReadStatusNotFound;
}


static ReadStatus readNumber(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    char *end = NULL;
    double num = strtod(*buf, &end);

    if (end == *buf) return kReadStatusNotFound;

    Token *tmp = (Token *) calloc (1, sizeof(Token));
    if (!tmp)   return kReadStatusNoMemory;

    if (pushBack(tokens, tmp) != SUCCESS)
        return kReadStatusNoMemory;

    free(tmp);

    *buf = end;

    return kReadStatusFound;
}


static ReadStatus readName(char **buf, Vector *tokens, Vector *names_table) {

    assert(buf);
    assert(*buf);
    assert(names_table);
    assert(tokens);

    char *name = getName(buf);

    NameType type = IfFuncBegins ? FUNC_NAME : VAR_NAME;

    int name_index = 0;
    if (type == FUNC_NAME) {
        if (!IfFunctionExists(name, data, &name_index)) {
            name_index = (int) data->size;
            if (PushName(data, {name, type}) != SUCCESS)
                return kReadStatusNoMemory;
        }
        else {
            free(name);
            name = NULL;
        }
        tokens->data[tokens->size - 1].func_index = name_index;
        return kReadStatusFound;
    }

    if (IfFunctionExists(name, data, &name_index)) {
        if (PushBack(tokens, {FUNCTION, {.func_index = name_index}}) != SUCCESS)
            return kReadStatusNoMemory;
        return kReadStatusFound;
    }

    if (!IfNameExists(name, data, &name_index)) {
        name_index = (int) data->size;

        if (PushName(data, {name, type}) != SUCCESS)
            return kReadStatusNoMemory;
    }
    else {
        free(name);
        name = NULL;
    }

    if (PushBack(tokens, {VARIABLE, {.var_index = name_index}}) != SUCCESS)
        return kReadStatusNoMemory;

    return kReadStatusFound;
}


static ReadStatus readString(char **buf, Vector *tokens) {

    assert(buf);
    assert(*buf);
    assert(tokens);

    size_t len_of_str = 0;
    while ((*buf)[len_of_str] != '\"' && (*buf)[len_of_str] != '\0')
        len_of_str++;

    char *str = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!str) return kReadStatusNoMemory;

    sscanf(*buf, "%[^\"]", str);

    Name *tmp = (Name *) calloc (1, sizeof(Name));
    if (!tmp) {
        free(str);
        return kReadStatusNoMemory;
    }
    tmp->name = str;
    tmp->type = STRING;

    if (pushBack(tokens, tmp) != SUCCESS)
        return kReadStatusNoMemory;

    free(tmp);

    *buf += len_of_str + 1;

    return kReadStatusFound;
}

static int PrototypesAnalysis(char **buf, NamesTable *data) {

    assert(buf);
    assert(*buf);
    assert(data);

    while (true) {

        skipSpaces(buf);
        skipComments(buf);
        skipSpaces(buf);

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

            while (**buf != ';' && **buf != '\0')
                *buf += 1;
            *buf += 1;`
        }

        *buf += 1;
    }
}

static bool IfNameExists(const char *str, NamesTable *data, int *name_index) {

    assert(str);
    assert(data);
    assert(name_index);
    assert(data->names);

    for (size_t i = data->start_search_index; i < data->size; i++) {
        if (strcmp(data->names[i].name, str) == 0 && data->names[i].type == VAR_NAME) {
            *name_index = (int) i;
            return true;
        }
    }

    return false;
}

static bool IfFunctionExists(const char *str, NamesTable *data, int *func_index) {

    assert(str);
    assert(data);
    assert(func_index);

    for (size_t i = NUMBER_OF_KEY_WORDS - 1; i < data->size; i++) {
        if (strcmp(str, data->names[i].name) == 0 && data->names[i].type == FUNC_NAME) {
            *func_index = (int) i;
            return true;
        }
    }

    return false;
}


static bool IsValidSymbol(const char sym) {

    const size_t PUNC_SYM_NUM = 15;
    char punct_symbols[PUNC_SYM_NUM] = {'(', ')', '{', '}', ';', '\n', ' ', '"', '*', '\\', '-', '+', '^', ' ', ','};

    for (size_t i = 0; i < PUNC_SYM_NUM; i++)
        if (sym == punct_symbols[i])
            return false;

    return true;
}

static void skipComments(char **buf) {

    assert(buf);
    assert(*buf);

    while (**buf == ' ' || **buf == '\n')
        *buf += 1;
}

static void skipComments(char **buf) {

    assert(buf);
    assert(*buf);

    if (**buf == '/' && *(*buf + 1) == '/') {
        *buf += 2;
        while (**buf != '\n')
            *buf += 1;
        *buf += 1;
    }
}

static int readName(char **buf, NameType type, Vector *names_table) {

    assert(buf);
    assert(*buf);
    assert(names_table);

    size_t len_of_str = 0;
    while (IsValidSymbol((*buf)[len_of_str]))
        len_of_str++;

    if (len_of_str == 0) {
        printf(RED "LA error: " END_OF_COLOR "invalid name \"%s\"\n", *buf);
        return ERROR;
    }

    char *str = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!str) return NO_MEMORY;

    sscanf(*buf, "%[^(){} \n\";-+*\\^,]", str);

    *buf += len_of_str;

    Name *tmp = (Name *) calloc (1, sizeof (Name));
    if (!tmp) {
        free(str);
        return NO_MEMORY;
    }
    tmp->name = str;
    tmp->type = type;

    if (pushBack(names_table, tmp) != SUCCESS)
        return NO_MEMORY;

    nameDtor(tmp);

    return SUCCESS;
}

void nameDtor(Name *name) {

    assert(name);

    free(&name->name);
    free(name);
}

void *getPtr(Vector *data, size_t index) {

    assert(data);

    return (data->data + index * data->element_size);
}