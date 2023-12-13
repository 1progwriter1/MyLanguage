#ifndef STRING_PARSE
#define STRING_PARSE

#include <stdio.h>
#include "my_vector.h"
#include "names_table.h"
#include "key_words.h"
#include "../diff_func/headers/bin_tree.h"

#define INPUT_FILE "expr.txt"

enum ParseError {
    NO_ERROR,
    INCORRECT_ENDING,
    CL_PARENTHESIS_MISSED,
    INCORRECT_OPERATION,
    EMPTY_STRING,
    MISSING_NUMBER,
};

struct StringParseData {
    Vector *tokens;
    size_t position;
    ParseError error;
};

int StringParse(NamesTable *names, Vector *tokens, TreeStruct *tree);

TreeNode *GetExpression(StringParseData *data, TreeStruct *tree);

TreeNode *GetTerm(StringParseData *data, TreeStruct *tree);

TreeNode *GetPrimaryExpression(StringParseData *data, TreeStruct *tree);

TreeNode *GetNumber(StringParseData *data, TreeStruct *tree);

TreeNode *GetUnary(StringParseData *data, TreeStruct *tree);

#endif