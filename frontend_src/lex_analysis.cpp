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
    kReadStatusError,
};

#define KEY_WORD_LEN strlen(getStrPtr(names_table, i))
#define IfFuncBegins (tokens->data[tokens->size - 1].type == FUNCTION)

#define BEGIN_OF_MAIN "once upon the time"
#define BEGIN_OF_FUNCTIONS "fairytale"

static int prepareToAnalyze  (Vector *names_table, Vector *tokens);
static int prototypesAnalysis(char **buf, Vector *data);

static ReadStatus readNumber     (char **buf, Vector *tokens);
static ReadStatus readString     (char **buf, Vector *tokens);
static ReadStatus readKeyWord    (char **buf, Vector *tokens, Vector *names_table);
static ReadStatus readPunctuation(char **buf, Vector *tokens);
static ReadStatus readName       (char **buf, Vector *tokens, Vector *names_table);


static bool ifNameExists    (const char *str, Vector *names_table, size_t *index);
static bool ifFunctionExists(const char *str, Vector *names_table, size_t *func_index);
static bool isValidSymbol   (const char sym);

static void skipSpaces  (char **buf);
static void skipComments(char **buf);

static const char *getName  (char **buf);

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

    Token *null_symbol = (Token *) calloc (1, sizeof(Token));
    if (!null_symbol)  return ERROR;

    null_symbol->type = PUNCT_SYM;
    null_symbol->sym_code = END_SYMBOL;

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


    if (pushBack(tokens, null_symbol) != SUCCESS)
        return ERROR;

    free(null_symbol);
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

            const char *func_name = getName(buf);
            if (!func_name) return ERROR;

            Name *tmp = (Name *) calloc (1, sizeof(Name));
            if (!tmp)   return ERROR;

            tmp->name = func_name;
            tmp->type = FUNC_NAME;

            if (pushBack(names_table, tmp) != SUCCESS)
                return ERROR;

            nameDtor(tmp);

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
        if (strncmp(*buf, getStrPtr(names_table, i), KEY_WORD_LEN) == 0) {

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

    tmp->type = NUMBER;
    tmp->number = num;

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

    const char *name = getName(buf);
    if (!name)  return kReadStatusError;

    Token *tmp_token = (Token *) calloc (1, sizeof(Token));
    if (!tmp_token)   return kReadStatusNoMemory;

    size_t index = 0;
    NameType type = ifFunctionExists(name, names_table, &index) ? FUNC_NAME : VAR_NAME;

    if (type == FUNC_NAME) {
        tmp_token->type = FUNCTION;
        tmp_token->func_index = index;
    }
    else {
        if (!ifNameExists(name, names_table, &index)) {
            Name *tmp = (Name *) calloc (1, sizeof(Name));
            if (!tmp)   return kReadStatusNoMemory;

            tmp->name = name;
            tmp->type = VAR_NAME;

            index = names_table->size - 1;
        }
        tmp_token->type = VARIABLE;
        tmp_token->var_index = index;
    }

    if (pushBack(tokens, tmp_token) != SUCCESS)
        return kReadStatusNoMemory;

    free(tmp_token);
    free((void *) name);

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

    Token *tmp = (Token *) calloc (1, sizeof(Token));
    if (!tmp) {
        free(str);
        return kReadStatusNoMemory;
    }
    tmp->type = STRING;
    tmp->string = str;

    if (pushBack(tokens, tmp) != SUCCESS)
        return kReadStatusNoMemory;

    free(tmp);
    free(str);

    *buf += len_of_str + 1;

    return kReadStatusFound;
}

static bool ifNameExists(const char *str, Vector *names_table, size_t *name_index) {

    assert(str);
    assert(names_table);
    assert(name_index);

    for (size_t i = NUMBER_OF_KEY_WORDS; i < names_table->size; i++) {
        if (strcmp(getStrPtr(names_table, i), str) == 0 && getNameType(names_table, i) == VAR_NAME) {
            *name_index = i;
            return true;
        }
    }

    return false;
}

static bool ifFunctionExists(const char *str, Vector *names_table, size_t *func_index) {

    assert(str);
    assert(names_table);
    assert(func_index);

    for (size_t i = NUMBER_OF_KEY_WORDS - 1; i < names_table->size; i++) {
        if (strcmp(str, getStrPtr(names_table, i)) == 0 && getNameType(names_table, i) == FUNC_NAME) {
            *func_index = i;
            return true;
        }
    }

    return false;
}


static bool isValidSymbol(const char sym) {

    const size_t PUNC_SYM_NUM = 15;
    char punct_symbols[PUNC_SYM_NUM] = {'(', ')', '{', '}', ';', '\n', ' ', '"', '*', '\\', '-', '+', '^', ' ', ','};

    for (size_t i = 0; i < PUNC_SYM_NUM; i++)
        if (sym == punct_symbols[i])
            return false;

    return true;
}

static void skipSpaces(char **buf) {

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

static const char *getName(char **buf) {

    assert(buf);
    assert(*buf);

    size_t len_of_str = 0;
    while (isValidSymbol((*buf)[len_of_str]))
        len_of_str++;

    if (len_of_str == 0) {
        printf(RED "LA error: " END_OF_COLOR "invalid name \"%s\"\n", *buf);
        return NULL;
    }

    char *str = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!str) return NULL;

    sscanf(*buf, "%[^(){} \n\";-+*\\^,]", str);

    *buf += len_of_str;

    return str;
}

void nameDtor(Name *name) {

    assert(name);

    free((void *) name->name);
    free(name);
}

const char *getStrPtr(Vector *names_table, size_t index) {

    assert(names_table);

    return ((Name *) ((char *) names_table->data + index * names_table->element_size))->name;
}

NameType getNameType(Vector *names_table, size_t index) {

    assert(names_table);

    return ((Name *) ((char *) names_table->data + index * names_table->element_size))->type;
}