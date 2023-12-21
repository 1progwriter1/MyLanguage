#include <stdio.h>
#include "../headers/gen_asm.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/key_words_codes.h"

#define CODE_GEN_ASSERT assert(node);                   \
                        assert(data);                   \
                        assert(data->vars_data.data);   \
                        assert(data->fn);

enum VarSearchStatus {
    kStatusFound,
    kStatusNotFound,
};

static int GenFunction(TreeNode *node, CodeGenData *data);
static int GenNewLine(TreeNode *node, CodeGenData *data);
static int GenIf(TreeNode *node, CodeGenData *data);
static int GenWhile(TreeNode *node, CodeGenData *data);
static int GenAssign(TreeNode *node, CodeGenData *data);
static int GenOutput(TreeNode *node, CodeGenData *data);
static int GenExpression(TreeNode *node, CodeGenData *data);
static int GenLogicalJump(TreeNode *node, CodeGenData *data);
static int GenUnaryOp(TreeNode *node, CodeGenData *data);
static int GenBinaryOp(TreeNode *node, CodeGenData *data);
static int GenInput(TreeNode *node, CodeGenData *data);
static int GenCall(TreeNode *node, CodeGenData *data);

static int DtorLocalVars(CodeGenData *data);
static int GetVarValue(size_t var_code, CodeGenData *data);
static int WriteVarValue(size_t var_code, CodeGenData *data);
static void CallMainPrint(FILE *fn);
static VarSearchStatus IfVariableExsists(size_t var_code, CodeGenData *data);
static int CreateVariable(CodeGenData *data, size_t var_index);
static int GenArgs(TreeNode *node, CodeGenData *data);

static bool IsPunct(TreeNode *node, Punctuation sym);
static bool IsBinOp(TreeNode *node, Binary_Op operation);
static bool IsUnOp(TreeNode *node, Unary_Op operation);
static bool IsKeyOp(TreeNode *node, Key_Op operation);
static bool IsType(TreeNode *node, ValueType type);

const char *REGS[] = {"rax", "rbx", "rcx", "rdx"};
const size_t NUM_OF_REGS = 4;
const size_t RAM_SIZE = 10000;

int GenAsmCode(TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    if (TreeVerify(tree) != SUCCESS)
        return ERROR;

    if (tree->root->value.type != FUNCTION) {
        printf(RED "asm gen error: " END_OF_COLOR "function expected\n");
        return ERROR;
    }

    FILE *fn = fileopen(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    VarsAddresses vars_data = {};
    if (VarsAddressesCtor(&vars_data) != SUCCESS) {
        fileclose(fn);
        return ERROR;
    }

    Stack local_vars = {};
    STACK_CTOR(local_vars)

    CallMainPrint(fn);

    CodeGenData data = {fn, 0, 0, 0, vars_data, local_vars, 0, 0};
    if (GenFunction(tree->root, &data) != SUCCESS)
        return ERROR;

    StackDtor(&local_vars);
    VarsAddressesDtor(&vars_data);
    fileclose(fn);

    return SUCCESS;
}

static int GenFunction(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.func_index == 0)
        fprintf(data->fn, ":main\n");
    else
        fprintf(data->fn, ":func_%lu\n", node->value.func_index);

    if (!node->left || !IsPunct(node->left, NEW_LINE)) {
        printf(RED "gen asm error: " END_OF_COLOR "fucntion body expected\n");
        return ERROR;
    }

    if (node->left->right && IsType(node->left->right, VARIABLE))
        if (GenArgs(node->left->right, data) != SUCCESS) return ERROR;

    if (StackPush(&data->local_vars, data->vars_data.size) != SUCCESS) return ERROR;

    if (node->left->left && IsPunct(node->left->left, NEW_LINE))
        if (GenNewLine(node->left->left, data) != SUCCESS) return ERROR;

    if (node->right && node->right->value.type == FUNCTION)
        if (GenFunction(node->right, data) != SUCCESS) return ERROR;

    if (DtorLocalVars(data) != SUCCESS) return ERROR;

    return SUCCESS;
}

static int GenArgs(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (CreateVariable(data, node->value.var_index) != SUCCESS)
        return ERROR;

    if (WriteVarValue(node->value.var_index, data) != SUCCESS)
        return ERROR;

    if (node->right && IsType(node->right, VARIABLE))
        if (GenArgs(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int GenNewLine(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left)
        return SUCCESS;

    if (IsKeyOp(node->left, IF))
        if (GenIf(node->left, data) != SUCCESS)
            return ERROR;

    if (IsKeyOp(node->left, WHILE))
        if (GenWhile(node->left, data) != SUCCESS)
            return ERROR;

    if (IsBinOp(node->left, ASSIGN))
        if (GenAssign(node->left, data) != SUCCESS)
            return ERROR;

    if (IsUnOp(node->left, OUT) || IsUnOp(node->left, OUT_S))
        if (GenOutput(node->left, data) != SUCCESS)
            return ERROR;

    if (IsUnOp(node->left, IN))
        if (GenInput(node->left, data) != SUCCESS)
            return ERROR;

    if (IsType(node->left, FUNCTION)) {
        if (GenCall(node->left->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall func_%lu\n", node->left->value.func_index);
    }

    if (IsUnOp(node->left, RET))
        fprintf(data->fn, "\t\tret\n");

    if (node->right && IsPunct(node->right, NEW_LINE))
        if (GenNewLine(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int GenInput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right || node->right->value.type != VARIABLE) {
        printf(RED "gen asm error: " END_OF_COLOR "variable for input expected\n");
        return ERROR;
    }

    fprintf(data->fn, "\t\tin\n");

    if (WriteVarValue(node->right->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int GenCall(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->right && IsType(node->right, VARIABLE))
        if (GenCall(node->right, data) != SUCCESS)
            return ERROR;

    if (GetVarValue(node->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int GenIf(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->left->left || !node->left->right) return ERROR;

    if (GenExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (GenExpression(node->left->left, data) != SUCCESS)  return ERROR;

    size_t if_index = data->cur_if++;

    if (GenLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "if_%lu\n", if_index);

    if (node->right->right && IsPunct(node->right->right, NEW_LINE))
        if (GenNewLine(node->right->right, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp end_if_%lu\n", if_index);
    fprintf(data->fn, ":if_%lu\n", if_index);

    if (node->right->left && IsPunct(node->right->left, NEW_LINE))
        if (GenNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, ":end_if_%lu\n", if_index);

    return SUCCESS;
}

static int GenWhile(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right) {
        printf(RED "gen asm error: " END_OF_COLOR "incorrect while struct\n");
        return ERROR;
    }
    size_t while_index = data->cur_while++;
    fprintf(data->fn, ":while_%lu\n", while_index);

    if (GenExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (GenExpression(node->left->left, data) != SUCCESS) return ERROR;

    if (GenLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "begin_while_%lu\n", while_index);

    fprintf(data->fn, "\t\tjmp end_while_%lu\n", while_index);

    fprintf(data->fn, ":begin_while_%lu\n", while_index);
    if (GenNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp while_%lu\n", while_index);

    fprintf(data->fn, ":end_while_%lu\n", while_index);

    return SUCCESS;
}

static int GenAssign(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right || (node->left->value.type != VARIABLE))
        return ERROR;

    if (GenExpression(node->right, data) != SUCCESS)
        return ERROR;

    if (IfVariableExsists(node->left->value.var_index, data) == kStatusNotFound) {

        if (CreateVariable(data, node->left->value.var_index) != SUCCESS)
            return ERROR;
    }
    if (WriteVarValue(node->left->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int GenOutput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right) return ERROR;

    Unary_Op operation = node->value.un_op;

    if (operation == OUT_S) {

        char *str = node->right->value.string;
        while (*str != '\0') {
            if (*str == '\\' && *(str + 1) == 'n') {
                fprintf(data->fn, "\t\tpush %d\n\t\toutc\n", '\n');
                str += 2;
            }
            else fprintf(data->fn, "\t\tpush %d\n\t\toutc\n", *str++);
        }
    }
    else {
        if (node->right->value.type == NUMBER)
            fprintf(data->fn, "\t\tpush %lg\n", node->right->value.number);
        else {
            if (GetVarValue(node->right->value.var_index, data) != SUCCESS)
                return ERROR;
        }
        fprintf(data->fn, "\t\tout\n");
    }

    return SUCCESS;
}

static int GenExpression(TreeNode *node, CodeGenData *data) {

    assert(data);
    assert(data->fn);

    if (!node)
        return SUCCESS;

    if (node->value.type == UNARY_OP) {
        if (GenExpression(node->right, data) != SUCCESS)
            return ERROR;

        if (GenUnaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }
    if (node->value.type == BINARY_OP) {
        if (GenExpression(node->left, data) != SUCCESS || GenExpression(node->right, data) != SUCCESS)
            return ERROR;

        if (GenBinaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }

    if (node->value.type == VARIABLE) {
        if (GetVarValue(node->value.var_index, data) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }
    if (node->value.type == NUMBER) {
        fprintf(data->fn, "\t\tpush %lg\n", node->value.number);
        return SUCCESS;
    }

    printf(RED "asm gen error: " END_OF_COLOR "invalid value type for assignment\n");

    return ERROR;

}

static int CreateVariable(CodeGenData *data, size_t var_index) {

    assert(data);
    assert(data->vars_data.data);

    if (data->cur_reg_ind < NUM_OF_REGS) {

        if (PushVarAddress(&data->vars_data, {var_index, VarPlaceREGS, data->cur_reg_ind++}) != SUCCESS)
            return ERROR;
    }
    else if (data->cur_ram_ind < RAM_SIZE) {

        if (PushVarAddress(&data->vars_data, {var_index, VarPlaceRAM, data->cur_ram_ind++}) != SUCCESS)
            return ERROR;
    }
    else {
        printf(RED "asm gen error: " END_OF_COLOR "unable to push variable\n");
        return ERROR;
    }

    return SUCCESS;

}

static void CallMainPrint(FILE *fn) {

    assert(fn);

    fprintf(fn, "\t\tcall main\n\t\thlt\n\n");
}

static int GetVarValue(size_t var_code, CodeGenData *data) {

    assert(data);
    assert(data->vars_data.data);

    for (size_t i = 0; i < data->vars_data.size; i++) {
        if (data->vars_data.data[i].var_code == var_code) {
            if (data->vars_data.data[i].place == VarPlaceRAM) {
                fprintf(data->fn, "\t\tpush [%lu]\n", data->vars_data.data[i].var_index);
            }
            else {
                fprintf(data->fn, "\t\tpush %s\n", REGS[data->vars_data.data[i].var_index]);
            }
            return SUCCESS;
        }
    }

    printf(RED "gen asm error: " END_OF_COLOR "variable does not exist\n");

    return ERROR;
}

static int WriteVarValue(size_t var_code, CodeGenData *data) {

    assert(data);
    assert(data->vars_data.data);

    for (size_t i = 0; i < data->vars_data.size; i++) {
        if (data->vars_data.data[i].var_code == var_code) {
            if (data->vars_data.data[i].place == VarPlaceRAM) {
                fprintf(data->fn, "\t\tpop [%lu]\n", data->vars_data.data[i].var_index);
            }
            else {
                fprintf(data->fn, "\t\tpop %s\n", REGS[data->vars_data.data[i].var_index]);
            }
            return SUCCESS;
        }
    }
    printf(RED "gen asm error: " END_OF_COLOR "variable to write value does not exist\n");

    return ERROR;
}

static int GenLogicalJump(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != BINARY_OP) {
        printf(RED "gen asm error: " END_OF_COLOR "binary operator expected\n");
        return ERROR;
    }

    if (node->value.type != BINARY_OP)
        return ERROR;

    if (node->value.bin_op == EQUAL)
        fprintf(data->fn, "\t\tje ");

    else if (node->value.bin_op == NOT_EQ)
        fprintf(data->fn, "\t\tjne ");

    else if (node->value.bin_op == ABOVE)
        fprintf(data->fn, "\t\tja ");

    else if (node->value.bin_op == BELOW)
        fprintf(data->fn, "\t\tjb ");

    else if (node->value.bin_op == AB_EQ)
        fprintf(data->fn, "\t\tjae ");

    else if (node->value.bin_op == BEl_EQ)
        fprintf(data->fn, "\t\tjbe ");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid logical operation\n");
        return ERROR;
    }

    return SUCCESS;
}

static int GenUnaryOp(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != UNARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "unary operation expected\n");
        return ERROR;
    }

    if (node->value.un_op == SIN)
        fprintf(data->fn, "\t\tsin\n");

    else if (node->value.un_op == COS)
        fprintf(data->fn, "\t\tcos\n");

    else if (node->value.un_op == LN)
        fprintf(data->fn, "\t\tln\n");

    else if (node->value.un_op == SQRT)
        fprintf(data->fn, "\t\tsqrt\n");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid unary operation\n");
        return ERROR;
    }

    return SUCCESS;
}

static int GenBinaryOp(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.type != BINARY_OP) {
        printf(RED "asm gen error: " END_OF_COLOR "binary operation expected\n");
        return ERROR;
    }

    if (node->value.bin_op == ADD)
        fprintf(data->fn, "\t\tadd\n");

    else if (node->value.bin_op == SUB)
        fprintf(data->fn, "\t\tsub\n");

    else if (node->value.bin_op == MUL)
        fprintf(data->fn, "\t\tmul\n");

    else if (node->value.bin_op == DIV)
        fprintf(data->fn, "\t\tdiv\n");

    else if (node->value.bin_op == POW)
        fprintf(data->fn, "\t\tpow\n");

    else {
        printf(RED "asm gen error: " END_OF_COLOR "invalid binary operation\n");
        return ERROR;
    }

    return SUCCESS;

}

static int DtorLocalVars(CodeGenData *data) {

    assert(data);

    size_t index = 0;
    if (StackPop(&data->local_vars, &index) != SUCCESS)
        return ERROR;

    Address value = {};
    while (data->vars_data.size != index)
        if (PopVarsAddresses(&data->vars_data, &value) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static VarSearchStatus IfVariableExsists(size_t var_code, CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < data->vars_data.size; i++) {
        if (var_code == data->vars_data.data[i].var_code)
            return kStatusFound;
    }

    return kStatusNotFound;
}

static bool IsPunct(TreeNode *node, Punctuation sym) {

    assert(node);

    return node->value.type == PUNCT_SYM && node->value.sym_code == sym;
}

static bool IsBinOp(TreeNode *node, Binary_Op operation) {

    assert(node);

    return node->value.type == BINARY_OP && node->value.bin_op == operation;
}

static bool IsUnOp(TreeNode *node, Unary_Op operation) {

    assert(node);

    return node->value.type == UNARY_OP && node->value.un_op == operation;
}

static bool IsKeyOp(TreeNode *node, Key_Op operation) {

    assert(node);

    return node->value.type == KEY_OP && node->value.key_op == operation;
}

static bool IsType(TreeNode *node, ValueType type) {

    assert(node);

    return node->value.type == type;
}