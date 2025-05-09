#include <assert.h>
#include <math.h>
#include "systemdata.h"
#include <string.h>
#include "file_func.h"
#include "parse.h"
#include "../middle_end_src/diff_dsl.h"
#include "lex_analysis.h"
#include "dump.h"
#include <stdlib.h>
#include "my_lang_lib.h"

/*
    G  ::= B '\0'
    F  ::= '(' (ARGS) ')' '{' B '}' ';'
    B  ::= K | A | O | I | C | R

    R ::= "end" '(' ( E ) ')' ';'
    ARGS ::= Variable ([','] Variable)*
    C ::= "function" '(' ')' ';'
    I ::= "guess the riddle" '(' Variable ')'
    K ::= "stone" Cond B B | "fell into a dead sleep while" Cond B
    O ::= say '(' N | S ')' ';'
    A ::= N 'turn into' E ';'
    Cond ::= '(' N ['turned into','not stronger','not weaker','stronger','weaker'] N ')'

    E  ::= T (['+','-'] T)*
    T  ::= P (['*,'/'] P)*
    P  ::= '(' E ')' | N
    N  ::= Number | Variable | C;
*/

#define PARSE_ASSERT    do {                        \
                            assert(data);           \
                            assert(data->tokens);   \
                            assert(tree);           \
                        } while (0);

#define PUNCT_ASSERT(sym, err, ptr1, ptr2)  do {                                \
                                                if (!isPunct(data, sym)) {      \
                                                    data->error = err;          \
                                                    RETURN_ON_ERROR(ptr1, ptr2) \
                                                }                               \
                                            } while (0);

#define RETURN_ON_ERROR(node1, node2) if (data->error != NO_ERROR)      \
                    {fprintf(stderr, "error on line: %d\n", __LINE__);  \
                    nodeDtor(tree, node1);  nodeDtor(tree, node2);      \
                    return NULL;}

#define BinaryOp(data) getTokenPtr(data, data->position)->bin_op
#define UnaryOp(data)  getTokenPtr(data, data->position)->un_op

const char *PARSE_ERRORS[] = {
[NO_ERROR]              = "no_error",
[OP_PARENTHESIS_MISSED] = "open parenthesis missed",
[CL_PARENTHESIS_MISSED] = "closing parenthesis missed",
[CL_BRACE_MISSED]       = "closing brace missed",
[KEY_OP_ERROR]          = "invalid key operator",
[LOGICAL_OP_ERROR]      = "logical operator expected",
[NUMBER_ERROR]          = "number, variable or call expected expected",
[ASSIGN_ERROR]          = "assign expected",
[NEW_LINE_ERROR]        = "; expected",
[COPY_ERROR]            = "string copy failed",
[MEMORY_ERROR]          = "node creation failed",
[UNARY_OP_ERROR]        = "sin, cos, ln or sqrt expected",
[CONDITION_ERROR]       = "condition expected",
[INPUT_ERROR]           = "variable for input expected",
[ARGS_ERROR]            = "argument after comma expected"
                        };

static bool isType(StringParseData *data, ValueType type);
static bool isBinOp(StringParseData *data, Binary_Op operation);
static bool isUnOp(StringParseData *data, Unary_Op operation);
static bool isPunct(StringParseData *data, Punctuation punct_sym);
static bool isKeyOp(StringParseData *data, Key_Op operation);

static ValueType getType(StringParseData *data);

static char *copyStr(char *src);

int stringParse(Vector *tokens, TreeStruct *tree, Vector *names_table) {

    assert(tokens);
    assert(tree);

    StringParseData data = {tokens, 0, NO_ERROR};
    size_t start_position = data.position;

    tree->root = getFunction(&data, tree);
    if (data.error != NO_ERROR) {
        printf(RED "error: " END_OF_COLOR "%s\nposition: %lu\n", PARSE_ERRORS[data.error], data.position);
        dumpErrors(&data, tokens, names_table);
        treeRootDtor(tree);
        return ERROR;
    }

    if (!isPunct(&data, END_SYMBOL)) {
        fprintf(stderr, "\n%lu\n", data.position);
        printf(RED "error: " END_OF_COLOR "expected: \\0\n");
        treeRootDtor(tree);
        return ERROR;
    }

    if (start_position == data.position) {
        printf(MAGENTA "warning: " END_OF_COLOR "empty expression\n");
    }

    if (treeVerify(tree) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

TreeNode *getFunction(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *func_ptr = NULL;

    if (getType(data) == FUNCTION) {
        func_ptr = NEW(FUNC(getTokenPtr(data, data->position)->func_index), NEW(PUNCT(NEW_LINE), NULL, NULL), NULL);
        data->position++;

        PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, func_ptr, NULL)
        data->position++;

        if (getTokenPtr(data, data->position)->func_index != 0) {
            TreeNode *args = getArgs(data, tree);
            func_ptr->left->right = args;
        }

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, func_ptr, NULL)
        data->position++;
    }
    else return NULL;

    if (!isPunct(data, OP_BRACE)) return NULL;
    data->position++;

    TreeNode *ptr = getBody(data, tree);
    RETURN_ON_ERROR(ptr, func_ptr)

    func_ptr->left->left = ptr;

    PUNCT_ASSERT(CL_BRACE, CL_BRACE_MISSED, func_ptr, NULL)
    data->position++;

    TreeNode *next = getFunction(data, tree);
    func_ptr->right = next;
    RETURN_ON_ERROR(func_ptr, NULL)

    return func_ptr;
}

TreeNode *getArgs(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (!isType(data, VARIABLE))
        return NULL;

    TreeNode *ptr = NEW(VAR(getTokenPtr(data, data->position)->var_index), NULL, NULL);
    data->position++;

    if (!isPunct(data, COMMA))
        return ptr;
    data->position++;

    if (!isType(data, VARIABLE)) {
        data->error = ARGS_ERROR;
        RETURN_ON_ERROR(ptr, NULL);
    }

    TreeNode *next = getArgs(data, tree);
    RETURN_ON_ERROR(ptr, next);
    ptr->right = next;

    return ptr;
}

TreeNode *getBody(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (data->error != NO_ERROR)
        return NULL;

    TreeNode *ptr = NULL;

    ptr = getKeyOp(data, tree);
    RETURN_ON_ERROR(ptr, NULL)

    if (!ptr) {
        ptr = getAssign(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
    }

    if (!ptr) {
        ptr = getOutput(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
    }

    if (!ptr) {
        ptr = getInput(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
    }

    if (!ptr) {
        ptr = getCall(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
    }

    if (!ptr) {
        ptr = getRet(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
    }

    if (!ptr) return NULL;

    TreeNode *next = getBody(data, tree);

    return NEW_S(PUNCT(NEW_LINE), ptr, next);

}

TreeNode *getRet(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr = NULL;
    if (isUnOp(data, RET)) {
        data->position++;
        ptr = NEW(UN_OP(RET), NULL, NULL);

        PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        if (!isPunct(data, CL_PARENTHESIS)) {
            ptr->right = getExpression(data, tree);
            RETURN_ON_ERROR(ptr, NULL)
        }

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        PUNCT_ASSERT(NEW_LINE, NEW_LINE_ERROR, ptr, NULL)
        data->position++;
    }

    return ptr;
}

TreeNode *getCall(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr = NULL;
    if (isType(data, FUNCTION)) {
        ptr = NEW(FUNC(getTokenPtr(data, data->position)->func_index), NULL, NULL);
    }
    else {
        return NULL;
    }
    data->position++;

    PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, ptr, NULL)
    data->position++;

    TreeNode *args = getArgsExpression(data, tree);
    RETURN_ON_ERROR(args, ptr);

    ptr->right = args;

    PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED ,ptr, NULL)
    data->position++;

    PUNCT_ASSERT(NEW_LINE, NEW_LINE_ERROR, ptr, NULL)
    data->position++;

    return ptr;
}

TreeNode *getInput(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (isUnOp(data, IN)) {
        data->position++;

        PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, NULL, NULL)
        data->position++;

        if (!isType(data, VARIABLE)) {
            data->error = INPUT_ERROR;
            return NULL;
        }

        TreeNode *ptr = getNumber(data, tree);
        RETURN_ON_ERROR(ptr, NULL)

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        PUNCT_ASSERT(NEW_LINE, NEW_LINE_ERROR, ptr, NULL)
        data->position++;

        return NEW_S(UN_OP(IN), NULL, ptr);
    }

    return NULL;
}

TreeNode *getOutput(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (isUnOp(data, OUT) || isUnOp(data, OUT_S)) {

        Unary_Op operation = isUnOp(data, OUT) ? OUT : OUT_S;
        data->position++;

        PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, NULL, NULL)
        data->position++;

        TreeNode *value = NULL;
        if (operation == OUT) {
            value = getNumber(data, tree);
        }
        else {
            char *str = copyStr(getTokenPtr(data, data->position)->string);
            if (!str) {
                data->error = COPY_ERROR;
                return NULL;
            }
            value = NEW(STR(str), NULL, NULL);
            data->position++;
        }

        RETURN_ON_ERROR(value, NULL)

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, value, NULL);
        data->position++;

        PUNCT_ASSERT(NEW_LINE, NEW_LINE_ERROR, value ,NULL)
        data->position++;

        return NEW_S(UN_OP(operation), NULL, value);
    }

    return NULL;
}

TreeNode *getKeyOp(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (!isKeyOp(data, IF) && !isKeyOp(data, WHILE))
        return NULL;

    Key_Op operation = isKeyOp(data, IF) ? IF : WHILE;

    data->position++;
    TreeNode *cond = getCondition(data, tree);
    RETURN_ON_ERROR(cond, NULL)

    if (!isPunct(data, OP_BRACE))
        return NEW_S(KEY_OP(operation), cond, NEW(PUNCT(NEW_LINE), NULL, NULL));
    data->position++;

    TreeNode *body_yes = getBody(data, tree);
    RETURN_ON_ERROR(cond, body_yes);

    PUNCT_ASSERT(CL_BRACE, CL_BRACE_MISSED, body_yes, cond)
    data->position++;

    if (!isPunct(data, OP_BRACE))
        return NEW_S(KEY_OP(operation), cond, NEW(PUNCT(NEW_LINE), body_yes, NULL));
    data->position++;

    TreeNode *body_no = getBody(data, tree);
    if (data->error != NO_ERROR) {
        nodeDtor(tree, cond);
        nodeDtor(tree, body_yes);
        nodeDtor(tree, body_no);
        return NULL;
    }

    if (!isPunct(data, CL_BRACE)) {
        data->error = CL_BRACE_MISSED;
        nodeDtor(tree, cond);
        nodeDtor(tree, body_yes);
        nodeDtor(tree, body_no);
        return NULL;
    }
    data->position++;

    TreeNode *node = NEW_S(KEY_OP(operation), cond, NEW_S(PUNCT(NEW_LINE), body_yes, body_no));

    return node;

}

TreeNode *getCondition(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, NULL, NULL)
    data->position++;

    TreeNode *left = getExpression(data, tree);
    RETURN_ON_ERROR(left, NULL)

    if (!isType(data, BINARY_OP) || (BinaryOp(data) < EQUAL || NOT_EQ < BinaryOp(data))) {
        data->error = LOGICAL_OP_ERROR;
        RETURN_ON_ERROR(left, NULL)
    }
    Binary_Op operation = BinaryOp(data);
    data->position++;

    TreeNode *right = getExpression(data, tree);
    RETURN_ON_ERROR(left, right)

    PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, left, right);
    data->position++;

    return NEW_S(BIN_OP(operation), left, right);
}

TreeNode *getAssign(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (getType(data) != VARIABLE)
        return NULL;

    TreeNode *var = getNumber(data, tree);
    RETURN_ON_ERROR(var, NULL)

    if (!isBinOp(data, ASSIGN)) {
        data->error = ASSIGN_ERROR;
        RETURN_ON_ERROR(var, NULL)
    }

    data->position++;

    TreeNode *value = getExpression(data, tree);
    RETURN_ON_ERROR(value, var)

    PUNCT_ASSERT(NEW_LINE, NEW_LINE_ERROR, var, value)

    data->position++;

    return NEW_S(BIN_OP(ASSIGN), var, value);
}

TreeNode *getExpression(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr_fst = getTerm(data, tree);
    RETURN_ON_ERROR(ptr_fst, NULL)
    if (!ptr_fst) return NULL;

    while (isBinOp(data, ADD) || isBinOp(data, SUB)) {

        Binary_Op operation = BinaryOp(data);
        data->position++;

        TreeNode *ptr_snd = getTerm(data, tree);
        RETURN_ON_ERROR(ptr_fst, ptr_snd)

        ptr_fst = NEW_S(BIN_OP(operation), ptr_fst, ptr_snd);

    }

    return ptr_fst;
}

TreeNode *getTerm(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr_fst = getPrimaryExpression(data, tree);
    RETURN_ON_ERROR(ptr_fst, NULL);
    if (!ptr_fst) return NULL;

    while (isBinOp(data, MUL) || isBinOp(data, DIV) || isBinOp(data, POW)) {

        Binary_Op operation = BinaryOp(data);
        data->position++;

        TreeNode *ptr_snd = getPrimaryExpression(data, tree);
        RETURN_ON_ERROR(ptr_fst, ptr_snd);
        ptr_fst = NEW(BIN_OP(operation), ptr_fst, ptr_snd);
    }

    return ptr_fst;
}

TreeNode *getPrimaryExpression(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (isPunct(data, OP_PARENTHESIS)) {

        data->position++;
        TreeNode *ptr = getExpression(data, tree);
        RETURN_ON_ERROR(ptr, NULL)

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        return ptr;
    }
    else {
        return getUnary(data, tree);
    }
}

TreeNode *getUnary(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr = NULL;
    if (!isType(data, UNARY_OP)) {
        ptr = getNumber(data, tree);
        RETURN_ON_ERROR(ptr, NULL)
        return ptr;
    }

    Unary_Op operation = UnaryOp(data);
    if (operation > LN) {
        data->error = UNARY_OP_ERROR;
        RETURN_ON_ERROR(NULL, NULL)
    }
    data->position++;

    PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, NULL, NULL);
    data->position++;

    ptr = getExpression(data, tree);
    RETURN_ON_ERROR(ptr, NULL);
    if (!ptr) return NULL;

    PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, ptr, NULL);
    data->position++;

    return NEW_S(UN_OP(operation), NULL, ptr);
}

TreeNode *getNumber(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (isType(data, NUMBER)) {
        return NEW(NUM(getTokenPtr(data, data->position++)->number), NULL, NULL);
    }
    else if (isType(data, VARIABLE)) {
        return NEW(VAR(getTokenPtr(data, data->position++)->var_index), NULL, NULL);
    }
    else if (isType(data, FUNCTION)) {

        TreeNode *ptr = NEW(FUNC(getTokenPtr(data, data->position)->func_index), NULL, NULL);
        data->position++;

        PUNCT_ASSERT(OP_PARENTHESIS, OP_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        ptr->right = getArgsExpression(data, tree);
        RETURN_ON_ERROR(ptr, NULL);

        PUNCT_ASSERT(CL_PARENTHESIS, CL_PARENTHESIS_MISSED, ptr, NULL)
        data->position++;

        return ptr;
    }
    else {
        data->error = NUMBER_ERROR;
        return NULL;
    }
}

TreeNode *getArgsExpression(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *arg = getExpression(data, tree);
    if (!arg) return NULL;

    TreeNode *ptr = NEW(PUNCT(NEW_LINE), arg, NULL);

    if (!isPunct(data, COMMA))
        return ptr;
    data->position++;

    TreeNode *arg_next = getArgsExpression(data, tree);
    if (!arg_next)  return ptr;

    ptr->right = arg_next;

    return ptr;
}

static char *copyStr(char *src) {

    assert(src);

    size_t len_of_str = strlen(src);

    char *dst = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!dst) return NULL;

    memcpy(dst, src, len_of_str);

    return dst;
}

static bool isType(StringParseData *data, ValueType type) {

    assert(data);
    assert(data->tokens);

    return getType(data) == type;
}

static bool isBinOp(StringParseData *data, Binary_Op operation) {

    assert(data);
    assert(data->tokens);

    return isType(data, BINARY_OP)  && getTokenPtr(data, data->position)->bin_op == operation;
}

static bool isUnOp(StringParseData *data, Unary_Op operation) {

    assert(data);
    assert(data->tokens);

    return isType(data, UNARY_OP) && getTokenPtr(data, data->position)->un_op == operation;
}

static bool isPunct(StringParseData *data, Punctuation punct_sym) {

    assert(data);
    assert(data->tokens);

    return isType(data, PUNCT_SYM) && getTokenPtr(data, data->position)->sym_code == punct_sym;
}

static bool isKeyOp(StringParseData *data, Key_Op operation) {

    assert(data);
    assert(data->tokens);

    return isType(data, KEY_OP) && getTokenPtr(data, data->position)->key_op == operation;
}

static ValueType getType(StringParseData *data) {

    assert(data);

    return getTokenPtr(data, data->position)->type;
}

Token *getTokenPtr(StringParseData *data, size_t index) {

    assert(data);

    return (Token *) getPtr(data->tokens, index);
}
