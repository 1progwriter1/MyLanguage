#include <stdio.h>
#include "../headers/file_read_lang.h"
#include "../../MyLibraries/headers/file_func.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include <string.h>
#include "../diff_func/headers/bin_tree.h"
#include "../headers/key_words_codes.h"

static int ReadNodeFile(TreeStruct *tree, TreeNode *node, FileBuffer *buffer);
static bool IsNewNode(FileBuffer *buffer);
static int GetValue(TreeNode *node, FileBuffer *buffer);
static int EndNode(FileBuffer *buffer);
static char *ReadStr(FileBuffer *buffer);
static void SkipSpaces(FileBuffer *buffer);
static int GetType(TreeNode *node, FileBuffer *buffer);

int ReadFileLang(TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    if (TreeRootCtor(tree) != SUCCESS)
        return ERROR;

    FileBuffer buffer = {};
    buffer.buf = readbuf(filename);
    if (!buffer.buf) {
        printf(RED "Buffer creation error" END_OF_COLOR "\n");
        return ERROR;
    }

    buffer.index = 0;

    if (IsNewNode(&buffer)) {
        if (ReadNodeFile(tree, tree->root, &buffer) != SUCCESS) {
            printf(RED "Error while reading the tree" END_OF_COLOR "\n");
            TreeRootDtor(tree);
            return ERROR;
        }
    }

    free(buffer.buf);

    if (TreeVerify(tree) != SUCCESS) {
        TreeRootDtor(tree);
        return ERROR;
    }

    return SUCCESS;
}

static int ReadNodeFile(TreeStruct *tree, TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(tree);
    assert(buffer);
    assert(buffer->buf);

    if (GetValue(node, buffer) != SUCCESS)
        return ERROR;

    if (IsNewNode(buffer)) {
        node->left = TreeNodeNew(tree, (Token) {}, NULL, NULL);
        if (!node->left) return NO_MEMORY;
        if (ReadNodeFile(tree, node->left, buffer) != SUCCESS)
            return ERROR;
    }

    if (IsNewNode(buffer)) {
        node->right = TreeNodeNew(tree, (Token) {}, NULL, NULL);
        if (!node->right) return NO_MEMORY;
        if (ReadNodeFile(tree, node->right, buffer) != SUCCESS)
            return ERROR;
    }

    if (EndNode(buffer) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int GetValue(TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(buffer);
    assert(buffer->buf);

    if (GetType(node, buffer) != SUCCESS)
        return ERROR;

    SkipSpaces(buffer);

    int sym_read = 0;
    size_t index = 0;
    double num = 0;
    char *str = NULL;

    if (node->value.type == STRING) {
        str = ReadStr(buffer);
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
            printf(RED "read error: " END_OF_COLOR "ivalid type\n");
            return ERROR;
        }
    }

    node->value = value;

    return SUCCESS;
}

static bool IsNewNode(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    SkipSpaces(buffer);

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

static int EndNode(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    SkipSpaces(buffer);

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

static char *ReadStr(FileBuffer *buffer) {

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

    sscanf(buffer->buf + buffer->index, "%[^\"]", string);
    buffer->index += len_of_str + 1;

    return string;
}

static void SkipSpaces(FileBuffer *buffer) {

    assert(buffer);
    assert(buffer->buf);

    while(buffer->buf[buffer->index] == ' ')
        buffer->index++;
}

static int GetType(TreeNode *node, FileBuffer *buffer) {

    assert(node);
    assert(buffer);
    assert(buffer->buf);

    SkipSpaces(buffer);

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