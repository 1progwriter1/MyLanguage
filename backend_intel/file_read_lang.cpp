#include <stdio.h>
#include "file_read_lang.h"
#include "../../MyLibraries/headers/file_func.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include <string.h>
#include "../bin_tree/bin_tree.h"
#include "../headers/key_words_codes.h"
#include "../frontend_src/lex_analysis.h"
#include <stdlib.h>

static int readNodeFile(TreeStruct *tree, TreeNode *node, FileBuffer *buffer);
static bool isNewNode(FileBuffer *buffer);
static int getValue(TreeNode *node, FileBuffer *buffer);
static int endNode(FileBuffer *buffer);
static char *readStr(FileBuffer *buffer);
static void skipSpaces(FileBuffer *buffer);
static int getType(TreeNode *node, FileBuffer *buffer);

int readFileLang(TreeStruct *tree, const char *filename, Vector *names_table) {

    assert(tree);
    assert(filename);

    if (treeRootCtor(tree) != SUCCESS)
        return ERROR;

    FileBuffer buffer = {};
    buffer.names_table = names_table;
    buffer.buf = readFileToBuffer(filename);
    if (!buffer.buf) {
        printf(RED "backend error: " END_OF_COLOR "buffer creation failed\n");
        return ERROR;
    }

    buffer.index = 0;

    if (isNewNode(&buffer)) {
        if (readNodeFile(tree, tree->root, &buffer) != SUCCESS) {
            printf(RED "backend error: " END_OF_COLOR "failed to read file\n");
            treeRootDtor(tree);
            return ERROR;
        }
    }

    free(buffer.buf);

    if (treeVerify(tree) != SUCCESS) {
        treeRootDtor(tree);
        return ERROR;
    }

    return SUCCESS;
}

static int readNodeFile(TreeStruct *tree, TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(tree);
    assert(buffer);
    assert(buffer->buf);

    if (getValue(node, buffer) != SUCCESS)
        return ERROR;

    if (isNewNode(buffer)) {
        node->left = treeNodeNew(tree, (Token) {}, NULL, NULL);
        if (!node->left) return NO_MEMORY;
        if (readNodeFile(tree, node->left, buffer) != SUCCESS)
            return ERROR;
    }

    if (isNewNode(buffer)) {
        node->right = treeNodeNew(tree, (Token) {}, NULL, NULL);
        if (!node->right) return NO_MEMORY;
        if (readNodeFile(tree, node->right, buffer) != SUCCESS)
            return ERROR;
    }

    if (endNode(buffer) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int getValue(TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(buffer);
    assert(buffer->buf);

    if (getType(node, buffer) != SUCCESS)
        return ERROR;

    skipSpaces(buffer);

    int sym_read = 0;
    size_t index = 0;
    double num = 0;
    char *str = NULL;

    if (node->value.type == STRING) {
        str = readStr(buffer);
        if (!str) {
            printf(RED "read error: " END_OF_COLOR "string read failed\n");
            return ERROR;
        }
    }
    else if (node->value.type == NUMBER) {
        if (sscanf(buffer->buf + buffer->index, "%lg %n", &num, &sym_read) != 1) {
            printf(RED "read error: " END_OF_COLOR "number expected: %s\n", buffer->buf + buffer->index);
            return ERROR;
        }
    }
    else if (node->value.type == VARIABLE) {
        str = readStr(buffer);
        if (!str) {
            printf(RED "read error: " END_OF_COLOR "string read failed\n");
            return ERROR;
        }
        Name *tmp = (Name *) calloc (1, sizeof(Name));
        if (!tmp)   return ERROR;
        tmp->name = str;
        tmp->type = VAR_NAME;
        if (pushBack(buffer->names_table, tmp) != SUCCESS)
            return ERROR;
        free(tmp);
        index = buffer->names_table->size - 1;
    }
    else if (node->value.type == FUNCTION) {
        str = readStr(buffer);
        if (!str) {
            printf(RED "read error: " END_OF_COLOR "string read failed\n");
            return ERROR;
        }
        Name *tmp = (Name *) calloc (1, sizeof(Name));
        if (!tmp)   return ERROR;
        tmp->name = str;
        tmp->type = FUNC_NAME;
        if (pushBack(buffer->names_table, tmp) != SUCCESS)
            return ERROR;
        free(tmp);
        index = buffer->names_table->size - 1;
    }
    else {
        if (sscanf(buffer->buf + buffer->index, "%lu %n", &index, &sym_read) != 1) {
                printf(RED "read error: " END_OF_COLOR "index expected: %s\n", buffer->buf + buffer->index);
                return ERROR;
            }
    }

    buffer->index += (size_t) sym_read;
    Token value = {};
    switch (node->value.type) {
        case (PUNCT_SYM): {
            value = (Token) {PUNCT_SYM, {.sym_code = (Punctuation) index}};
            break;
        }
        case (BINARY_OP): {
            value = (Token) {BINARY_OP, {.bin_op = (Binary_Op) index}};
            break;
        }
        case (UNARY_OP): {
            value = (Token) {UNARY_OP, {.un_op = (Unary_Op) index}};
            break;
        }
        case (KEY_OP): {
            value = (Token) {KEY_OP, {.key_op = (Key_Op) index}};
            break;
        }
        case (VARIABLE): {
            value = (Token) {VARIABLE, {.var_index = index}};
            break;
        }
        case (FUNCTION): {
            value = (Token) {FUNCTION, {.func_index = index}};
            break;
        }
        case (STRING): {
            value = (Token) {STRING, {.string = str}};
            break;
        }
        case (NUMBER): {
            value = (Token) {NUMBER, {.number = num}};
            break;
        }
        default: {
            printf(RED "read error: " END_OF_COLOR "invalid type\n");
            return ERROR;
        }
    }

    node->value = value;

    return SUCCESS;
}

static bool isNewNode(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    skipSpaces(buffer);

    char *ptr = buffer->buf + buffer->index;

    if (*ptr == '(') {
        buffer->index++;
        return true;
    }
    else if (*ptr == '_') {
        buffer->index++;
        return false;
    }
    else {
        printf(RED "New node expected: " END_OF_COLOR "\"%s\"\n", ptr);
        return false;
    }

}

static int endNode(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    skipSpaces(buffer);

    char *ptr = buffer->buf + buffer->index;

    if (*ptr == ')') {
        buffer->index++;
        return SUCCESS;
    }
    else {
        printf(RED "End of node expected: " END_OF_COLOR "\"%s\"\n", ptr);
            return ERROR;
        }
}

static char *readStr(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    if (buffer->buf[buffer->index] != '"')
        return NULL;

    buffer->index++;

    size_t len_of_str = 0;
    while (buffer->buf[buffer->index + len_of_str] != '"')
        len_of_str++;

    char *string = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!string) return NULL;

    memcpy(string, buffer->buf + buffer->index, len_of_str * sizeof(char));
    *(string + len_of_str) = '\0';
    buffer->index += len_of_str + 1;

    return string;
}

static void skipSpaces(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    while(buffer->buf[buffer->index] == ' ')
        buffer->index++;
}

static int getType(TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(buffer);
    assert(buffer->buf);

    skipSpaces(buffer);

    char *ptr = buffer->buf + buffer->index;

    int type = 0;
    if (sscanf(ptr, "%d", &type) != 1) {
        printf(RED "read error: " END_OF_COLOR "%s\n", ptr);
        return ERROR;
    }
    if (type > STRING) {
        printf(RED "read error: " END_OF_COLOR "ivalid type\n");
        return ERROR;
    }
    buffer->index++;

    node->value.type = (ValueType) type;

    return SUCCESS;
}