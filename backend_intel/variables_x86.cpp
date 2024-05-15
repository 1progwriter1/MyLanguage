#include "variables_x86.h"
#include "gen_asm_x86.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../lib_src/my_lang_lib.h"
#include <stdlib.h>

const Registers ARGUMENTS_SRC[] = {RDI, RSI, RDX, RCX, R8, R9};
const char *REGS_NAMES[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

int prepareData(CodeGenData *data, const char *filename, Vector *names_table) {

    assert(data);
    assert(names_table);
    assert(filename);

    FILE *fn = openFile(filename, WRITE);
    if (!fn)    return FILE_OPEN_ERROR;

    data->fn = fn;
    data->indexes.cur_if = 0;
    data->indexes.cur_while = 0;
    data->indexes.cur_stack_ind = 0;
    data->indexes.cur_func_exe = 0;

    setRegisters(data);

    if (vectorCtor(data->vars.variables, 8, sizeof(Address)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        return ERROR;
    }
    if (vectorCtor(data->str_data, 8, sizeof(char **)) != SUCCESS) {
        closeFile(fn);
        vectorDtor(names_table);
        vectorDtor(data->vars.variables);
        return ERROR;
    }

    return SUCCESS;
}

void dtorData(CodeGenData *data) {

    assert(data);

    closeFile(data->fn);

    data->indexes.cur_if = 0;
    data->indexes.cur_while = 0;
    data->indexes.cur_stack_ind = 0;
    data->indexes.cur_func_exe = 0;

    vectorDtor(data->vars.variables);
    vectorDtor(data->str_data);
}

int createSegment(CodeGenData *data, TreeNode *node) {

    assert(data);

    if (!node)  return SUCCESS;

    data->indexes.cur_stack_ind = 0;
    size_t arg_index = 0;

    Address *variable = (Address *) calloc (1, sizeof(Address));
    if (!variable)  return ERROR;

    fprintf(data->fn, ";save arguments to memory\n");
    TreeNode *cur_arg = node;
    while (cur_arg) {
        if (arg_index >= 6) {
            printf("too many args");
            break;
        }
        if (!isType(node, VARIABLE)) {
            printf(RED "segment creation: " END_OF_COLOR "variable expected\n");
            free(variable);
            return ERROR;
        }

        if (writeVariable(data, cur_arg, {.type = TypeReg, .reg = ARGUMENTS_SRC[arg_index]}) != SUCCESS)    return ERROR;
        cur_arg = cur_arg->right;
        arg_index++;
    }

    free(variable);

    return SUCCESS;
}

int destroySegment(CodeGenData *data) {

    assert(data);

    data->indexes.cur_stack_ind = 0;
    long int tmp = ftell(data->fn);
    fseek(data->fn, data->offset, SEEK_SET);
    fprintf(data->fn, "%lu", data->vars.variables->size * VALUE_SIZE);
    fseek(data->fn, tmp, SEEK_SET);

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

int writeVariable(CodeGenData *data, TreeNode *node, ValueSrc src) {

    assert(data);

    if (!isType(node, VARIABLE)) {
        printf(RED "write variable: " END_OF_COLOR "variable expected\n");
        return ERROR;
    }

    size_t var_index = 0;
    bool found = false;
    for (size_t i = 0; i < data->vars.variables->size; i++) {
        if (node->value.var_index == getVarIndex(data, i)) {
           var_index = i;
           found  = true;
           break;
        }
    }
    if (!found) {
        Address *new_var = (Address *) calloc (1, sizeof (Address));
        if (!new_var)   return ERROR;

        new_var->var_code = node->value.var_index;
        new_var->var_index = data->indexes.cur_stack_ind++;

        if (pushBack(data->vars.variables, new_var) != SUCCESS) {
            free(new_var);
            printf(RED "write variable: " END_OF_COLOR "failed to create variable\n");
            return ERROR;
        }

        var_index = new_var->var_index;
        free(new_var);
    }

    fprintf(data->fn, "\t\tmov qword [rbp - %lu], ", (var_index + 1) * VALUE_SIZE);
    if (src.type == TypeReg)          fprintf(data->fn, "%s", REGS_NAMES[src.reg]);
    else if (src.type == TypeStack)   fprintf(data->fn, "qword [rbp - %lu]", (src.index + 1) * VALUE_SIZE);
    else                              fprintf(data->fn, "%lld", src.number);
    fprintf(data->fn, "\t\t;write value [%s]\n", getStrPtr(data->vars.names_table, node->value.var_index));

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
            fprintf(data->fn, "qword [rbp - %lu]", (getVarPlace(data, i) + 1) * VALUE_SIZE);
            return SUCCESS;
        }
    }

    return ERROR;
}

void setRbp(CodeGenData *data) {

    assert(data);

    fprintf(data->fn,   "\t\tpush rbp\n"
                        "\t\tmov rbp, rsp\n"
                        "\t\tsub rsp, ");
    data->offset = ftell(data->fn);
    fprintf(data->fn, "\t\t\t\t\t;allocate memory\n");
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

void setRegisters(CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < NUMBER_OF_USED; i++)
        data->vars.used_regs[i] = {(Registers) i, false};
}

bool findVariable(CodeGenData *data, size_t *index, size_t code) {

    assert(data);
    assert(index);

    for (size_t i = 0; i < data->vars.variables->size; i++) {
        if (code == getVarIndex(data, i)) {
            *index = getVarPlace(data, i);
            return true;
        }
    }

    return false;
}

int findFreeRegister(CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < data->vars.variables->size; i++) {
        if (data->vars.used_regs[i].is_used)
            continue;
        return (int) i;
    }

    return -1;
}

void printPlace(CodeGenData *data, ValueSrc src) {

    assert(data);

    switch (src.type) {
        case (TypeNumber): {
            fprintf(data->fn, "%lld", src.number);
            break;
        }
        case (TypeReg): {
            fprintf(data->fn, "%s", REGS_NAMES[src.reg]);
            break;
        }
        case (TypeStack): {
            fprintf(data->fn, "[rbp - %lu]", (src.index + 1) * VALUE_SIZE);
            break;
        }
        default:
            break;
    }

}