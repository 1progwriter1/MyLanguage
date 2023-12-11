#include <assert.h>
#include <math.h>
#include "../../MyLibraries/headers/systemdata.h"
#include <string.h>
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/parse.h"
#include "../headers/parse_dsl.h"
#include "../diff_func/headers/diff_dsl.h"

int StringParse(NamesTable *names, Vector *tokens, TreeStruct *tree) {

    assert(tokens);
    assert(tree);
    assert(names);

    StringParseData data = {tokens, 0, NO_ERROR};
    size_t start_position = data.position;

    if (TreeRootCtor(tree) != SUCCESS) return ERROR;

    tree->root = GetExpression(&data, tree);
    if (!tree->root) return ERROR;

    if (tokens->data[data.position].type != TOKEN_END_SYM) {
        printf(RED "Syntax error: " END_OF_COLOR "\nexpected: \\0\n");
        TreeRootDtor(tree);
        return ERROR;
    }

    if (data.error != NO_ERROR) {
        TreeRootDtor(tree);
        return ERROR;
    }

    if (start_position == data.position) {
        printf(MAGENTA "warning: " END_OF_COLOR "empty expression\n");
    }

    return SUCCESS;
}

TreeNode *GetExpression(StringParseData *data, TreeStruct *tree) {

    assert(data);
    assert(data->tokens);
    assert(tree);

    TreeNode *ptr_fst = GetTerm(data, tree);
    if (!ptr_fst) return NULL;

    while (IsAdd(data->position) || IsSub(data->position)) {

        size_t old_position = data->position;
        data->position += 1;

        TreeNode *ptr_snd = GetTerm(data, tree);
        if (!ptr_snd) return NULL;


        if (IsAdd(old_position)) {
            return NEW_S(OP(ADD), ptr_fst, ptr_snd);
        }
        if (IsSub(old_position)) {
            return NEW_S(OP(SUB), ptr_fst, ptr_snd);
        }
    }
    return ptr_fst;
}

TreeNode *GetTerm(StringParseData *data, TreeStruct *tree) {

    assert(data);
    assert(data->tokens);
    assert(tree);

    TreeNode *ptr_fst = GetPrimaryExpression(data, tree);
    if (!ptr_fst) return NULL;

    while (IsMul(data->position) || IsDiv(data->position)) {

        size_t old_position = data->position;
        data->position += 1;

        TreeNode *ptr_snd = GetPrimaryExpression(data, tree);
        if (!ptr_snd) return NULL;

        if (IsMul(old_position)) {
            return NEW_S(OP(MUL), ptr_fst, ptr_snd);
        }
        if (IsDiv(old_position)) {
            return NEW_S(OP(DIV), ptr_fst, ptr_snd);
        }
    }

    return ptr_fst;
}

TreeNode *GetPrimaryExpression(StringParseData *data, TreeStruct *tree) {

    assert(data);
    assert(data->tokens);
    assert(tree);

    if (IsOpBracket(data->position)) {

        data->position += 1;
        TreeNode *ptr = GetExpression(data, tree);
        if (!ptr) return NULL;

        PAR_ASSERT

        data->position += 1;

        return ptr;
    }
    else {
        return GetUnary(data, tree);
    }
}

TreeNode *GetUnary(StringParseData *data, TreeStruct *tree) {

    assert(data);
    assert(data->tokens);
    assert(tree);

    TreeNode *ptr = NULL;
    if (IsCos(data->position)) {
        data->position += 1;
        if (IsOpBracket(data->position)) {
            ptr = NEW_S(OP(COS), NULL, GetExpression(data, tree));
        }
    }
    if (IsSin(data->position)) {
        data->position += 1;
        if (IsOpBracket(data->position)) {
            ptr = NEW_S(OP(COS), NULL, GetExpression(data, tree));
        }
    }

    PAR_ASSERT

    if (ptr)
        return ptr;

    return GetNumber(data, tree);
}

TreeNode *GetNumber(StringParseData *data, TreeStruct *tree) {

    assert(data);
    assert(data->tokens);
    assert(tree);

    if (IsNumber(data->position)) {
        return NEW(NUM(data->tokens->data[data->position++].number), NULL, NULL);
    }
    else {
        data->error = MISSING_NUMBER;
        return NULL;
    }
}

