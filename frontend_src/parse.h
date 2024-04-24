#ifndef STRING_PARSE_FUNCTIONS
#define STRING_PARSE_FUNCTIONS

#include <stdio.h>
#include "../../MyLibraries/headers/my_vector.h"
#include "../headers/key_words_codes.h"
#include "../bin_tree/bin_tree.h"

#define INPUT_FILE "expr.txt"


enum ParseError {
    NO_ERROR,
    OP_PARENTHESIS_MISSED,
    CL_PARENTHESIS_MISSED,
    CL_BRACE_MISSED,
    KEY_OP_ERROR,
    LOGICAL_OP_ERROR,
    NUMBER_ERROR,
    ASSIGN_ERROR,
    NEW_LINE_ERROR,
    COPY_ERROR,
    MEMORY_ERROR,
    UNARY_OP_ERROR,
    CONDITION_ERROR,
    INPUT_ERROR,
    ARGS_ERROR,
};

struct StringParseData {
    Vector *tokens;
    size_t position;
    ParseError error;
};

int stringParse(Vector *tokens, TreeStruct *tree);

TreeNode *getFunction         (StringParseData *data, TreeStruct *tree);

TreeNode *getExpression       (StringParseData *data, TreeStruct *tree);

TreeNode *getTerm             (StringParseData *data, TreeStruct *tree);

TreeNode *getPrimaryExpression(StringParseData *data, TreeStruct *tree);

TreeNode *getNumber           (StringParseData *data, TreeStruct *tree);

TreeNode *getUnary            (StringParseData *data, TreeStruct *tree);

TreeNode *getKeyOp            (StringParseData *data, TreeStruct *tree);

TreeNode *getAssign           (StringParseData *data, TreeStruct *tree);

TreeNode *getCondition        (StringParseData *data, TreeStruct *tree);

TreeNode *getOutput           (StringParseData *data, TreeStruct *tree);

TreeNode *getBody             (StringParseData *data, TreeStruct *tree);

TreeNode *getInput            (StringParseData *data, TreeStruct *tree);

TreeNode *getCall             (StringParseData *data, TreeStruct *tree);

TreeNode *getRet              (StringParseData *data, TreeStruct *tree);

TreeNode *getArgs             (StringParseData *data, TreeStruct *tree);

#endif