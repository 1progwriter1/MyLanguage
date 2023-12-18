#include <assert.h>
#include <math.h>
#include "../../MyLibraries/headers/systemdata.h"
#include <string.h>
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/parse.h"
#include "../diff_func/headers/diff_dsl.h"

/*
    G  ::= B '\0'
    F  ::= '{' B '}' ';'
    B  ::= K | A | O

    K ::= "stone" Cond B B | "fell into a dead sleep while" Cond B
    O ::= say '(' N | S ')' ';'
    A  ::= N 'turn into' E ';'
    Cond ::= '(' N ['turned into','not stronger','not weeker','stronger','weeker'] N ')'

    E  ::= T (['+','-'] T)*
    T  ::= P (['*,'/'] P)*
    P  ::= '(' E ')' | N
    N  ::= Number | Variable
*/

#define PARSE_ASSERT    do {                        \
                            assert(data);           \
                            assert(data->tokens);   \
                            assert(tree);           \
                        } while (0);

#define RETURN_ON_ERROR(node) if (data->error != NO_ERROR) \
                    {fprintf(stderr, "error on line: %d\n", __LINE__); NodeDtor(tree, node); return NULL;}

#define BinaryOp(data) data->tokens->data[data->position].bin_op
#define UnaryOp(data)  data->tokens->data[data->position].un_op


const char *PARSE_ERRORS[] = {
[NO_ERROR]              = "no_error",
[OP_PARENTHESIS_MISSED] = "open parenthesis missed",
[CL_PARENTHESIS_MISSED] = "closing parenthesis missed",
[CL_BRACE_MISSED]       = "closing brace missed",
[KEY_OP_ERROR]          = "invalid key operator",
[LOGICAL_OP_ERROR]      = "logical operator expected",
[NUMBER_ERROR]          = "number or variable expected",
[ASSIGN_ERROR]          = "assign expected",
[NEW_LINE_ERROR]        = "; expected",
[COPY_ERROR]            = "string copy failed",
[MEMORY_ERROR]          = "node creation failed",
[UNARY_OP_ERROR]        = "sin, cos, ln or sqrt expected",
[CONDITION_ERROR]       = "condition expected"
                        };

static bool IsType(StringParseData *data, ValueType type);
static bool IsBinOp(StringParseData *data, Binary_Op operation);
static bool IsUnOp(StringParseData *data, Unary_Op operation);
static bool IsPunct(StringParseData *data, Punctuation punct_sym);
static bool IsKeyOp(StringParseData *data, Key_Op operation);
static char *CopyStr(char *src);

int StringParse(Vector *tokens, TreeStruct *tree) {

    assert(tokens);
    assert(tree);

    StringParseData data = {tokens, 0, NO_ERROR};
    size_t start_position = data.position;

    tree->root = GetFunction(&data, tree);
    if (data.error != NO_ERROR) {
        printf(RED "error: " END_OF_COLOR "%s\nposition: %lu\n", PARSE_ERRORS[data.error], data.position);
        TreeRootDtor(tree);
        return ERROR;
    }

    if (!IsPunct(&data, END_SYMBOL)) {
        fprintf(stderr, "\n%lu\n", data.position);
        printf(RED "error: " END_OF_COLOR "expected: \\0\n");
        TreeRootDtor(tree);
        return ERROR;
    }

    if (start_position == data.position) {
        printf(MAGENTA "warning: " END_OF_COLOR "empty expression\n");
    }

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

TreeNode *GetFunction(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *func_ptr = NULL;

    if (data->tokens->data[data->position].type == FUNCTION) {
        func_ptr = NEW(FUNC(data->tokens->data[data->position].func_index), NULL, NULL);
        data->position++;
    }
    else return NULL;

    if (!IsPunct(data, OP_BRACE))
        return NULL;

    data->position++;

    TreeNode *ptr = GetBody(data, tree);
    RETURN_ON_ERROR(ptr);

    func_ptr->left = ptr;

    if (!IsPunct(data, CL_BRACE)) {
        data->error  =CL_BRACE_MISSED;
        NodeDtor(tree, func_ptr);
        return NULL;
    }

    data->position++;

    TreeNode *next = GetFunction(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, next);
        NodeDtor(tree, ptr);
        return NULL;
    }

    func_ptr->right = next;

    return func_ptr;
}

TreeNode *GetBody(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (data->error != NO_ERROR)
        return NULL;

    TreeNode *ptr = NULL;

    ptr = GetKeyOp(data, tree);
    RETURN_ON_ERROR(ptr)

    if (!ptr) {
        ptr = GetAssign(data, tree);
        RETURN_ON_ERROR(ptr)
    }

    if (!ptr) {
        ptr = GetOutput(data, tree);
        RETURN_ON_ERROR(ptr)
    }

    if (!ptr) return NULL;

    TreeNode *next = GetBody(data, tree);

    return NEW_S(PUNCT(NEW_LINE), ptr, next);

}

TreeNode *GetOutput(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (IsUnOp(data, OUT) || IsUnOp(data, OUT_S)) {
        Unary_Op operation = IsUnOp(data, OUT) ? OUT : OUT_S;
        data->position++;
        if (!IsPunct(data, OP_PARENTHESIS)) {
            data->error = OP_PARENTHESIS_MISSED;
            return NULL;
        }
        data->position++;

        TreeNode *value = NULL;
        if (operation == OUT) {
            value = GetNumber(data, tree);
        }
        else {
            char *str = CopyStr(data->tokens->data[data->position].string);
            if (!str) {
                data->error = COPY_ERROR;
                return NULL;
            }
            value = NEW(STR(str), NULL, NULL);
            data->position++;
        }

        RETURN_ON_ERROR(value)

        if (!IsPunct(data, CL_PARENTHESIS)) {
            NodeDtor(tree, value);
            data->error = CL_PARENTHESIS_MISSED;
            return NULL;
        }
        data->position++;

        if (!IsPunct(data, NEW_LINE)) {
            NodeDtor(tree, value);
            data->error = NEW_LINE_ERROR;
            return NULL;
        }
        data->position++;

        return NEW_S(UN_OP(operation), NULL, value);
    }

    return NULL;
}

TreeNode *GetKeyOp(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (!IsKeyOp(data, IF) && !IsKeyOp(data, WHILE))
        return NULL;

    Key_Op operation = IsKeyOp(data, IF) ? IF : WHILE;

    data->position++;
    TreeNode *cond = GetCondition(data, tree);
    RETURN_ON_ERROR(cond)
    if (!cond)

    if (!IsPunct(data, OP_BRACE))
        return NEW_S(KEY_OP(operation), cond, NEW(PUNCT(NEW_LINE), NULL, NULL));
    data->position++;

    TreeNode *body_yes = GetBody(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, cond);
        NodeDtor(tree, body_yes);
        return NULL;
    }
    if (!IsPunct(data, CL_BRACE)) {
        data->error = CL_BRACE_MISSED;
        NodeDtor(tree, body_yes);
        return NULL;

    }
    data->position++;

    TreeNode *body_no = NULL;
    if (!IsPunct(data, OP_BRACE))
        return NEW_S(KEY_OP(operation), cond, NEW(PUNCT(NEW_LINE), body_yes, NULL));
    data->position++;

    body_no = GetBody(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, cond);
        NodeDtor(tree, body_yes);
        NodeDtor(tree, body_no);
        return NULL;
    }

    if (!IsPunct(data, CL_BRACE)) {
        data->error = CL_BRACE_MISSED;
        NodeDtor(tree, body_yes);
        NodeDtor(tree, body_no);
        return NULL;
    }
    data->position++;


    TreeNode *node = NEW_S(KEY_OP(operation), cond, NEW_S(PUNCT(NEW_LINE), body_yes, body_no));

    return node;

}

TreeNode *GetCondition(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (!IsPunct(data, OP_PARENTHESIS)) {
        data->error = CONDITION_ERROR;
        return NULL;
    }
    data->position++;

    TreeNode *left = GetExpression(data, tree);
    RETURN_ON_ERROR(left)

    if (!IsType(data, BINARY_OP) || (BinaryOp(data) < EQUAL || NOT_EQ < BinaryOp(data))) {
        data->error = LOGICAL_OP_ERROR;
        NodeDtor(tree, left);
        return NULL;
    }
    Binary_Op operation = BinaryOp(data);
    data->position++;

    TreeNode *right = GetExpression(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, left);
        NodeDtor(tree, right);
        return NULL;
    }

    if (!IsPunct(data, CL_PARENTHESIS)) {
        data->error = CL_PARENTHESIS_MISSED;
        NodeDtor(tree, left);
        NodeDtor(tree, right);
        return NULL;
    }
    data->position++;

    return NEW_S(BIN_OP(operation), left, right);
}

TreeNode *GetAssign(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (data->tokens->data[data->position].type != VARIABLE)
        return NULL;

    TreeNode *var = GetNumber(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, var);
        return NULL;
    }

    if (!IsBinOp(data, ASSIGN)) {
        NodeDtor(tree, var);
        data->error = ASSIGN_ERROR;
        return NULL;
    }

    data->position++;

    TreeNode *value = GetExpression(data, tree);
    if (data->error != NO_ERROR) {
        NodeDtor(tree, value);
        NodeDtor(tree, var);
        return NULL;
    }

    if (!IsPunct(data, NEW_LINE)) {
        NodeDtor(tree, var);
        NodeDtor(tree, value);
        data->error = NEW_LINE_ERROR;
        return NULL;
    }

    data->position++;

    return NEW_S(BIN_OP(ASSIGN), var, value);
}

TreeNode *GetExpression(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr_fst = GetTerm(data, tree);
    RETURN_ON_ERROR(ptr_fst)
    if (!ptr_fst) return NULL;

    while (IsBinOp(data, ADD) || IsBinOp(data, SUB)) {

        Binary_Op operation = BinaryOp(data);
        data->position++;

        TreeNode *ptr_snd = GetTerm(data, tree);
        if (data->error != NO_ERROR || !ptr_snd) {
            NodeDtor(tree, ptr_fst);
            NodeDtor(tree, ptr_snd);
            return NULL;
        }
        if (!ptr_snd) {
            NodeDtor(tree, ptr_snd);
            return NULL;
        }

        if (operation == ADD) {
            return NEW_S(BIN_OP(ADD), ptr_fst, ptr_snd);
        }
        if (operation == SUB) {
            return NEW_S(BIN_OP(SUB), ptr_fst, ptr_snd);
        }
    }
    return ptr_fst;
}

TreeNode *GetTerm(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr_fst = GetPrimaryExpression(data, tree);
    RETURN_ON_ERROR(ptr_fst)
    if (!ptr_fst) return NULL;

    while (IsBinOp(data, MUL) || IsBinOp(data, DIV)) {

        Binary_Op operation = BinaryOp(data);
        data->position++;

        TreeNode *ptr_snd = GetPrimaryExpression(data, tree);
        if (data->error != NO_ERROR || !ptr_snd) {
            NodeDtor(tree, ptr_fst);
            NodeDtor(tree, ptr_snd);
            return NULL;
        }

        if (operation == MUL) {
            return NEW_S(BIN_OP(MUL), ptr_fst, ptr_snd);
        }
        if (operation == DIV) {
            return NEW_S(BIN_OP(DIV), ptr_fst, ptr_snd);
        }
    }

    return ptr_fst;
}

TreeNode *GetPrimaryExpression(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (IsPunct(data, OP_PARENTHESIS)) {

        data->position++;
        TreeNode *ptr = GetExpression(data, tree);
        if (data->error != NO_ERROR || !ptr) {
            NodeDtor(tree, ptr);
            return NULL;
        }

        data->position++;

        return ptr;
    }
    else {
        return GetUnary(data, tree);
    }
}

TreeNode *GetUnary(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    TreeNode *ptr = NULL;
    if (!IsType(data, UNARY_OP)) {
        ptr = GetNumber(data, tree);
        if (data->error != NO_ERROR || !ptr) {
            NodeDtor(tree, ptr);
            return NULL;
        }
        return ptr;
    }

    Unary_Op operation = UnaryOp(data);
    if (operation > LN) {
        data->error = UNARY_OP_ERROR;
        return NULL;
    }
    data->position++;

    if (!IsPunct(data, OP_PARENTHESIS)) {
        data->error = OP_PARENTHESIS_MISSED;
        return NULL;
    }
    data->position++;

    ptr = GetExpression(data, tree);
    if (data->error != NO_ERROR || !ptr) {
            NodeDtor(tree, ptr);
            return NULL;
        }

    if (!IsPunct(data, CL_PARENTHESIS)) {
        data->error = CL_PARENTHESIS_MISSED;
        NodeDtor(tree, ptr);
        return NULL;
    }
    data->position++;

    return NEW_S(UN_OP(operation), NULL, ptr);
}

TreeNode *GetNumber(StringParseData *data, TreeStruct *tree) {

    PARSE_ASSERT

    if (IsType(data, NUMBER)) {
        return NEW(NUM(data->tokens->data[data->position++].number), NULL, NULL);
    }
    else if (IsType(data, VARIABLE)) {
        return NEW(VAR(data->tokens->data[data->position++].var_index), NULL, NULL);
    }
    else {
        data->error = NUMBER_ERROR;
        return NULL;
    }
}

static char *CopyStr(char *src) {

    assert(src);

    size_t len_of_str = strlen(src);

    char *dst = (char *) calloc (len_of_str + 1, sizeof (char));
    if (!dst) return NULL;


    memcpy(dst, src, len_of_str);

    return dst;
}


static bool IsType(StringParseData *data, ValueType type) {

    assert(data);
    assert(data->tokens);

    return data->tokens->data[data->position].type == type;
}

static bool IsBinOp(StringParseData *data, Binary_Op operation) {

    assert(data);
    assert(data->tokens);

    return IsType(data, BINARY_OP)  && data->tokens->data[data->position].bin_op == operation;
}

static bool IsUnOp(StringParseData *data, Unary_Op operation) {

    assert(data);
    assert(data->tokens);

    return IsType(data, UNARY_OP) && data->tokens->data[data->position].un_op == operation;
}

static bool IsPunct(StringParseData *data, Punctuation punct_sym) {

    assert(data);
    assert(data->tokens);

    return IsType(data, PUNCT_SYM) && data->tokens->data[data->position].sym_code == punct_sym;
}

static bool IsKeyOp(StringParseData *data, Key_Op operation) {

    assert(data);
    assert(data->tokens);

    return IsType(data, KEY_OP) && data->tokens->data[data->position].key_op == operation;
}