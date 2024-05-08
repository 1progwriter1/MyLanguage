#include "variables.h"
#include "gen_asm.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../lib_src/my_lan_lib.h"

const char *REGS[] = {"rax", "rbx", "rcx", "rdx"};
// const size_t NUM_OF_REGS = 4;
const size_t RAM_SIZE = 10000;

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

    if (vectorCtor(data->vars.segments, 8, sizeof(Segment)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        return ERROR;
    }

    if (vectorCtor(data->vars.variables, 8, sizeof(Address)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        vectorDtor(data->vars.segments);
        return ERROR;
    }

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
        fprintf(data->fn,  "\t\tpop [%lu]\t\t;[%s] save to ram\n", data->indexes.cur_ram_ind, getStrPtr(data->vars.names_table, cur_arg->value.var_index));

        variable->var_code = cur_arg->value.var_index;
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

int writeVariable(CodeGenData *data, TreeNode *node) {

    assert(data);

    if (!isType(node, VARIABLE)) {
        printf(RED "write variable: " END_OF_COLOR "variable expected\n");
        return ERROR;
    }

    for (size_t i = getSegment(data); i < data->vars.variables->size; i++) {
        if (node->value.var_index == getVarIndex(data, i)) {
            fprintf(data->fn, "\t\tpop [%lu]\t\t;fill variable [%s]\n", getVarPlace(data, i), getStrPtr(data->vars.names_table, node->value.var_index));
            return SUCCESS;
        }
    }

    if (data->indexes.cur_ram_ind >= RAM_SIZE) {
        printf(RED "error: " END_OF_COLOR "memory limit exceeded\n");
        return ERROR;
    }

    Address *new_var = (Address *) calloc (1, sizeof (Address));
    if (!new_var)   return ERROR;

    new_var->place = VarPlaceRAM;
    new_var->var_code = node->value.var_index;
    new_var->var_index = data->indexes.cur_ram_ind++;

    if (pushBack(data->vars.variables, new_var) != SUCCESS) {
        printf(RED "write variable: " END_OF_COLOR "failed to create variable\n");
        return ERROR;
    }

    assert(data->indexes.cur_ram_ind - 1 == data->vars.variables->size - 1);

    fprintf(data->fn, "\t\tpop [%lu]\t\t;fill variable [%s]\n", data->indexes.cur_ram_ind - 1, getStrPtr(data->vars.names_table, node->value.var_index));

    free(new_var);

    return SUCCESS;
}

size_t getSegment(CodeGenData *data) {

    assert(data);

    size_t segment = ((Segment *) getPtr(data->vars.segments, data->vars.segments->size - 1))->segment;
    return segment;
}

size_t getVarIndex(CodeGenData *data, size_t index) {

    assert(data);

    return ((Address *) getPtr(data->vars.variables, index))->var_code;
}

size_t getVarPlace(CodeGenData *data, size_t index) {

    assert(data);

    return ((Address *) getPtr(data->vars.variables, index))->var_index;
}

int getVariableValue(CodeGenData *data, size_t var_code) {

    assert(data);

    for (size_t i = getSegment(data); i < data->vars.variables->size; i++) {
        if (var_code == getVarIndex(data, i)) {
            fprintf(data->fn, "\t\tpush [%lu]\t\t;get value [%s]\n", getVarPlace(data, i), getStrPtr(data->vars.names_table, var_code));
            return SUCCESS;
        }
    }

    printf(RED "get variable value: " END_OF_COLOR "variable does not exist\n");

    return ERROR;
}

int saveArgs(CodeGenData *data) {

    assert(data);


}

int restoreArgs(CodeGenData *data) {

    assert(data);
}