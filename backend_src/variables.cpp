#include "variables.h"
#include "gen_asm.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../lib_src/my_lan_lib.h"

int prepareData(CodeGenData *data, const char *filename, Vector *names_table) {

    assert(data);
    assert(names_table);
    assert(filename);

    FILE *fn = openFile(filename, WRITE);
    if (!fn)    return FILE_OPEN_ERROR;

    data->fn = fn;
    data->indexes.cur_if = 0;
    data->indexes.cur_while = 0;
    data->indexes.cur_ram_ind = 0;
    data->indexes.cur_reg_ind = 0;
    data->indexes.cur_func_exe = 0;

    Vector segments = {};
    if (vectorCtor(&segments, 8, sizeof(Segment)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        return ERROR;
    }

    Vector variables = {};
    if (vectorCtor(&variables, 8, sizeof(Address)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        vectorDtor(&segments);
        return ERROR;
    }

    data->vars.names_table = names_table;
    data->vars.segments = &segments;
    data->vars.variables = &variables;

    return SUCCESS;
}

void dtorData(CodeGenData *data) {

    assert(data);

    closeFile(data->fn);

    data->indexes.cur_if = 0;
    data->indexes.cur_while = 0;
    data->indexes.cur_ram_ind = 0;
    data->indexes.cur_reg_ind = 0;
    data->indexes.cur_func_exe = 0;

    vectorDtor(data->vars.names_table);
    vectorDtor(data->vars.segments);
    vectorDtor(data->vars.variables);

}

int createSegment(CodeGenData *data, TreeNode *node) {

    assert(data);

    Segment *tmp = (Segment *) calloc (1, sizeof(Segment));
    if (!tmp)   return ERROR;

    tmp->ram_index = data->indexes.cur_ram_ind;
    tmp->segment = data->vars.variables->size;
    if (pushBack(data->vars.segments, tmp) != SUCCESS) {
        free(tmp);
        return ERROR;
    }

    free(tmp);

    if (!node)  return SUCCESS;

    Address *variable = (Address *) calloc (1, sizeof(Address));
    if (!variable)  return ERROR;

    TreeNode *cur_arg = node;
    while (cur_arg) {
        if (!isType(node, VARIABLE)) {
            printf(RED "segment creation: " END_OF_COLOR "variable expected\n");
            free(variable);
            return ERROR;
        }
        printf(";[%s] save to ram\n", getStrPtr(data->vars.names_table, node->value.var_index));
        printf("\t\tpop [%lu]\n", data->indexes.cur_ram_ind);

        variable->var_code = node->value.var_index;
        variable->place = VarPlaceRAM;
        variable->var_index = data->indexes.cur_ram_ind++;

        if (pushBack(data->vars.variables, variable) != SUCCESS) {
            free(variable);
            return ERROR;
        }
        cur_arg = cur_arg->right;
    }

    free(variable);

    return SUCCESS;
}

int destroySegment(CodeGenData *data) {

    assert(data);

    Segment *tmp = (Segment *) calloc (1, sizeof(Segment));
    if (!tmp)   return ERROR;

    if (pop(data->vars.segments, tmp) != SUCCESS) {
        free(tmp);
        return ERROR;
    }

    data->indexes.cur_ram_ind = tmp->ram_index;

    Address *variable = (Address *) calloc (1, sizeof(Address));
    if (!variable) {
        free(tmp);
        return ERROR;
    }

    while (data->vars.variables->size > tmp->segment) {
        if (pop(data->vars.variables, variable) != SUCCESS) {
            free(tmp);
            free(variable);
            return ERROR;
        }
    }

    free(tmp);
    free(variable);

    return SUCCESS;
}