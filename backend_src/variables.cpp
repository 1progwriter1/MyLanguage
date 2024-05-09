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

    if (vectorCtor(data->vars.variables, 8, sizeof(Address)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
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
    vectorDtor(data->vars.variables);

}

int createSegment(CodeGenData *data, TreeNode *node) {

    assert(data);

    if (!node)  return SUCCESS;

    data->indexes.cur_ram_ind = 0;

    Address *variable = (Address *) calloc (1, sizeof(Address));
    if (!variable)  return ERROR;

    fprintf(data->fn, ";save arguments to memory\n");
    TreeNode *cur_arg = node;
    while (cur_arg) {
        if (!isType(node, VARIABLE)) {
            printf(RED "segment creation: " END_OF_COLOR "variable expected\n");
            free(variable);
            return ERROR;
        }
        if (writeVariable(data, cur_arg) != SUCCESS)    return ERROR;
        cur_arg = cur_arg->right;
    }

    free(variable);

    return SUCCESS;
}

int destroySegment(CodeGenData *data) {

    assert(data);

    data->indexes.cur_ram_ind = 0;

    Address *variable = (Address *) calloc (1, sizeof(Address));
    if (!variable) {
        return ERROR;
    }

    while (data->vars.variables->size > 0) {
        if (pop(data->vars.variables, variable) != SUCCESS) {
            free(variable);
            return ERROR;
        }
    }

    free(variable);

    return SUCCESS;
}

int writeVariable(CodeGenData *data, TreeNode *node) {

    assert(data);

    if (!isType(node, VARIABLE)) {
        printf(RED "write variable: " END_OF_COLOR "variable expected\n");
        return ERROR;
    }

    for (size_t i = 0; i < data->vars.variables->size; i++) {
        if (node->value.var_index == getVarIndex(data, i)) {
            setRbx(data, getVarPlace(data, i));
            fprintf(data->fn, "\t\tpop [rbx]\t\t;write value [%s]\n", getStrPtr(data->vars.names_table, node->value.var_index));
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

    setRbx(data, data->indexes.cur_ram_ind - 1);
    fprintf(data->fn, "\t\tpop [rbx]\t\t;fill variable [%s]\n", getStrPtr(data->vars.names_table, node->value.var_index));
    incRdx(data);

    free(new_var);

    return SUCCESS;
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

    for (size_t i = 0; i < data->vars.variables->size; i++) {
        if (var_code == getVarIndex(data, i)) {
            setRbx(data, getVarPlace(data, i));
            fprintf(data->fn, "\t\tpush [rbx]\t\t;get value [%s]\n", getStrPtr(data->vars.names_table, var_code));
            return SUCCESS;
        }
    }

    return ERROR;
}

int saveArgs(CodeGenData *data, TreeNode *node) {

    assert(data);

    if (!isType(node, VARIABLE))  return SUCCESS;

    if (node->right) saveArgs(data, node->right);

    getVariableValue(data, node->value.var_index);

    return SUCCESS;
}

int restoreArgs(CodeGenData *data, TreeNode *node) {

    assert(data);

    TreeNode *cur = node;
    while (isType(cur, VARIABLE)) {
        writeVariable(data, cur);
        cur = cur->right;
    }

    return SUCCESS;
}

void zeroRegs(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, ";zero registers\n");
    fprintf(data->fn, "\t\tpush 0\n\t\tpush 0\n");
    fprintf(data->fn, "\t\tpop rcx\n\t\tpop rdx\n\n");

}

void setSegment(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, ";set segment\n");
    fprintf(data->fn, "\t\tpush rcx\n\t\tpush rdx\n\t\tadd\n\t\tpop rcx\n\t\tpush 0\n\t\tpop rdx\n\n");
}

void incRdx(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, ";inc rdx\n");
    fprintf(data->fn, "\t\tpush 1\n\t\tpush rdx\n\t\tadd\n\t\tpop rdx\n\n");
}

void setRbx(CodeGenData *data, size_t index) {

    assert(data);

    fprintf(data->fn, ";set variable index [%lu]\n", index);
    fprintf(data->fn, "\t\tpush %lu\n\t\tpush rcx\n\t\tadd\n\t\tpop rbx\n\n", index);
}

void saveRdxRcx(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, "\t\tpush rcx\n\t\tpush rdx\n");
}

void restoreRdxRcx(CodeGenData *data) {

    assert(data);

    fprintf(data->fn, "\t\tpop rdx\n\t\tpop rcx\n");
}