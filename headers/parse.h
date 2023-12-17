#ifndef STRING_PARSE
#define STRING_PARSE

#include <stdio.h>
#include "my_vector.h"
#include "key_words_codes.h"
#include "../diff_func/headers/bin_tree.h"

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

TreeNode *GetIf(StringParseData *data, TreeStruct *tree);

TreeNode *GetWhile (StringParseData *data, TreeStruct *tree);

TreeNode *GetAssign(StringParseData *data, TreeStruct *tree);

TreeNode *GetCondition(StringParseData *data, TreeStruct *tree);

TreeNode *GetOutput(StringParseData *data, TreeStruct *tree);

TreeNode *GetBody(StringParseData *data, TreeStruct *tree);


#endif