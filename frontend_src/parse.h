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

int StringParse(Vector *tokens, TreeStruct *tree);

TreeNode *GetFunction(StringParseData *data, TreeStruct *tree);

TreeNode *GetExpression(StringParseData *data, TreeStruct *tree);

TreeNode *GetTerm(StringParseData *data, TreeStruct *tree);

TreeNode *GetPrimaryExpression(StringParseData *data, TreeStruct *tree);

TreeNode *GetNumber(StringParseData *data, TreeStruct *tree);

TreeNode *GetUnary(StringParseData *data, TreeStruct *tree);

TreeNode *GetKeyOp(StringParseData *data, TreeStruct *tree);

TreeNode *GetAssign(StringParseData *data, TreeStruct *tree);

TreeNode *GetCondition(StringParseData *data, TreeStruct *tree);

TreeNode *GetOutput(StringParseData *data, TreeStruct *tree);

TreeNode *GetBody(StringParseData *data, TreeStruct *tree);

TreeNode *GetInput(StringParseData *data, TreeStruct *tree);

TreeNode *GetCall(StringParseData *data, TreeStruct *tree);

TreeNode *GetRet(StringParseData *data, TreeStruct *tree);

TreeNode *GetArgs(StringParseData *data, TreeStruct *tree);

#endif