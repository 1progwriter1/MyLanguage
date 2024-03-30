#include <stdio.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../headers/prepare_front_rev.h"
#include <assert.h>

int PrepareToProgGen(TreeNode *node, FILE *fn, NamesTable *names) {

    assert(node);
    assert(fn);
    assert(names);

    if (NamesTableCtor(names) != SUCCESS)
        return ERROR;

    return SUCCESS;
}