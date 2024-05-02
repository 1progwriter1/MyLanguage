#include <stdio.h>
#include "gen_asm.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"
#include "../headers/key_words_codes.h"

#define CODE_GEN_ASSERT assert(node);                   \
                        assert(data);                   \
                        assert(data->fn);

enum VarSearchStatus {
    kStatusFound,
    kStatusNotFound,
};

static int genFunction(TreeNode *node, CodeGenData *data);
static int genNewLine(TreeNode *node, CodeGenData *data);
static int genIf(TreeNode *node, CodeGenData *data);
static int genWhile(TreeNode *node, CodeGenData *data);
static int genAssign(TreeNode *node, CodeGenData *data);
static int genOutput(TreeNode *node, CodeGenData *data);
static int genExpression(TreeNode *node, CodeGenData *data);
static int genLogicalJump(TreeNode *node, CodeGenData *data);
static int genUnaryOp(TreeNode *node, CodeGenData *data);
static int genBinaryOp(TreeNode *node, CodeGenData *data);
static int genInput(TreeNode *node, CodeGenData *data);
static int genCall(TreeNode *node, CodeGenData *data);
static int genRet(TreeNode *node, CodeGenData *data);

static int dtorLocalVars(CodeGenData *data);
static int getVarValue(size_t var_code, CodeGenData *data);
static int writeVarValue(size_t var_code, CodeGenData *data);
static void callMainPrint(FILE *fn);
static VarSearchStatus ifVariableExists(size_t var_code, CodeGenData *data);
static int createVariable(CodeGenData *data, size_t var_index);
static int genArgs(TreeNode *node, CodeGenData *data);

static bool isPunct(TreeNode *node, Punctuation sym);
static bool isBinOp(TreeNode *node, Binary_Op operation);
static bool isUnOp(TreeNode *node, Unary_Op operation);
static bool isKeyOp(TreeNode *node, Key_Op operation);
static bool isType(TreeNode *node, ValueType type);

const char *REGS[] = {"rax", "rbx", "rcx", "rdx"};
const size_t NUM_OF_REGS = 4;
const size_t RAM_SIZE = 10000;

int genAsmCode(TreeStruct *tree, const char *filename) {

    assert(tree);
    assert(filename);

    if (treeVerify(tree) != SUCCESS)
        return ERROR;

    if (tree->root->value.type != FUNCTION) {
        printf(RED "asm gen error: " END_OF_COLOR "function expected\n");
        return ERROR;
    }

    FILE *fn = openFile(filename, WRITE);
    if (!fn) return FILE_OPEN_ERROR;

    Vector vars_data = {};
    if (vectorCtor(&vars_data, 8, sizeof(Address)) != SUCCESS) {
        closeFile(fn);
        return ERROR;
    }

    Vector local_vars = {};
    if (vectorCtor(&local_vars, 8, sizeof(size_t)) != SUCCESS)
        return ERROR;

    callMainPrint(fn);

    CodeGenData data = {fn, 0, 0, 0, &vars_data, &local_vars, 0, 0, 0};
    if (genFunction(tree->root, &data) != SUCCESS)
        return ERROR;

    vectorDtor(&local_vars);
    vectorDtor(&vars_data);
    closeFile(fn);

    return SUCCESS;
}

static int genFunction(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->value.func_index == 0)
        fprintf(data->fn, ":main\n");
    else
        fprintf(data->fn, ":func_%lu\n", node->value.func_index);

    data->cur_func_exe = node->value.func_index;

    if (!node->left || !isPunct(node->left, NEW_LINE)) {
        printf(RED "gen asm error: " END_OF_COLOR "function body expected\n");
        return ERROR;
    }

    if (node->left->right && isType(node->left->right, VARIABLE))
        if (genArgs(node->left->right, data) != SUCCESS) return ERROR;

    size_t *tmp = (size_t *) calloc (1, sizeof(size_t));
    *tmp = data->vars_data->size;

    if (pushBack(data->local_vars, tmp) != SUCCESS) return ERROR;

    free(tmp);

    if (node->left->left && isPunct(node->left->left, NEW_LINE))
        if (genNewLine(node->left->left, data) != SUCCESS) return ERROR;

    if (node->right && node->right->value.type == FUNCTION)
        if (genFunction(node->right, data) != SUCCESS) return ERROR;

    if (dtorLocalVars(data) != SUCCESS) return ERROR;

    return SUCCESS;
}

static int genArgs(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (createVariable(data, node->value.var_index) != SUCCESS)
        return ERROR;

    if (writeVarValue(node->value.var_index, data) != SUCCESS)
        return ERROR;

    if (node->right && isType(node->right, VARIABLE))
        if (genArgs(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int genNewLine(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left)
        return SUCCESS;

    if (isKeyOp(node->left, IF))
        if (genIf(node->left, data) != SUCCESS)
            return ERROR;

    if (isKeyOp(node->left, WHILE))
        if (genWhile(node->left, data) != SUCCESS)
            return ERROR;

    if (isBinOp(node->left, ASSIGN))
        if (genAssign(node->left, data) != SUCCESS)
            return ERROR;

    if (isUnOp(node->left, OUT) || isUnOp(node->left, OUT_S))
        if (genOutput(node->left, data) != SUCCESS)
            return ERROR;

    if (isUnOp(node->left, IN))
        if (genInput(node->left, data) != SUCCESS)
            return ERROR;

    if (isType(node->left, FUNCTION)) {
        if (genCall(node->left->right, data) != SUCCESS)
            return ERROR;
    }

    if (isUnOp(node->left, RET))
        if (genRet(node->left, data) != SUCCESS)
            return ERROR;

    if (node->right && isPunct(node->right, NEW_LINE))
        if (genNewLine(node->right, data) != SUCCESS)
            return ERROR;

    return SUCCESS;
}

static int genRet(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->right) {
        if (genExpression(node->right, data) != SUCCESS)
            return ERROR;
    }

    fprintf(data->fn, "\t\tret\n");

    return SUCCESS;
}

static int genInput(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->right || node->right->value.type != VARIABLE) {
        printf(RED "gen asm error: " END_OF_COLOR "variable for input expected\n");
        return ERROR;
    }

    fprintf(data->fn, "\t\tin\n");

    if (writeVarValue(node->right->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int genCall(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (node->right && isType(node->right, VARIABLE)) {
        if (genCall(node->right, data) != SUCCESS)
            return ERROR;
    }

    if (getVarValue(node->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int genIf(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->left->left || !node->left->right) return ERROR;

    if (genExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data) != SUCCESS)  return ERROR;

    size_t if_index = data->cur_if++;

    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "if_%lu\n", if_index);

    if (node->right->right && isPunct(node->right->right, NEW_LINE))
        if (genNewLine(node->right->right, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp end_if_%lu\n", if_index);
    fprintf(data->fn, ":if_%lu\n", if_index);

    if (node->right->left && isPunct(node->right->left, NEW_LINE))
        if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, ":end_if_%lu\n", if_index);

    return SUCCESS;
}

static int genWhile(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right) {
        printf(RED "gen asm error: " END_OF_COLOR "incorrect while struct\n");
        return ERROR;
    }
    size_t while_index = data->cur_while++;
    fprintf(data->fn, ":while_%lu\n", while_index);

    if (genExpression(node->left->right, data) != SUCCESS) return ERROR;
    if (genExpression(node->left->left, data) != SUCCESS) return ERROR;

    if (genLogicalJump(node->left, data) != SUCCESS) return ERROR;
    fprintf(data->fn, "begin_while_%lu\n", while_index);

    fprintf(data->fn, "\t\tjmp end_while_%lu\n", while_index);

    fprintf(data->fn, ":begin_while_%lu\n", while_index);
    if (genNewLine(node->right->left, data) != SUCCESS) return ERROR;

    fprintf(data->fn, "\t\tjmp while_%lu\n", while_index);

    fprintf(data->fn, ":end_while_%lu\n", while_index);

    return SUCCESS;
}

static int genAssign(TreeNode *node, CodeGenData *data) {

    CODE_GEN_ASSERT

    if (!node->left || !node->right || (node->left->value.type != VARIABLE))
        return ERROR;

    if (genExpression(node->right, data) != SUCCESS)
        return ERROR;

    if (ifVariableExists(node->left->value.var_index, data) == kStatusNotFound) {

        if (createVariable(data, node->left->value.var_index) != SUCCESS)
            return ERROR;
    }
    if (writeVarValue(node->left->value.var_index, data) != SUCCESS)
        return ERROR;

    return SUCCESS;
}

static int genOutput(TreeNode *node, CodeGenData *data) {

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
            if (getVarValue(node->right->value.var_index, data) != SUCCESS)
                return ERROR;
        }
        fprintf(data->fn, "\t\tout\n");
    }

    return SUCCESS;
}

static int genExpression(TreeNode *node, CodeGenData *data) {

    assert(data);
    assert(data->fn);

    if (!node)
        return SUCCESS;

    if (isType(node, UNARY_OP)) {
        if (genExpression(node->right, data) != SUCCESS)
            return ERROR;

        if (genUnaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }
    if (isType(node, BINARY_OP)) {

        if (genExpression(node->left, data) != SUCCESS || genExpression(node->right, data) != SUCCESS)
                return ERROR;

        if (genBinaryOp(node, data) != SUCCESS)
            return ERROR;

        return SUCCESS;
    }

    if (isType(node, VARIABLE)) {
        if (getVarValue(node->value.var_index, data) != SUCCESS)
            return ERROR;
        return SUCCESS;
    }
    if (isType(node, NUMBER)) {
        fprintf(data->fn, "\t\tpush %lg\n", node->value.number);
        return SUCCESS;
    }

    if (isType(node, FUNCTION)) {
        if (genCall(node->right, data) != SUCCESS)
            return ERROR;
        fprintf(data->fn, "\t\tcall func_%lu\n", node->value.func_index);
        return SUCCESS;
    }

    printf(RED "asm gen error: " END_OF_COLOR "invalid value type for expression\n");

    return ERROR;

}

static int createVariable(CodeGenData *data, size_t var_index) {

    assert(data);

    Address *tmp = (Address *) calloc (1, sizeof(Address));
    if (!tmp)   return ERROR;
    tmp->var_index = var_index;


    if (data->cur_reg_ind < NUM_OF_REGS) {

        tmp->place = VarPlaceREGS;
        tmp->var_code = data->cur_reg_ind++;

        if (pushBack(data->vars_data, tmp) != SUCCESS)
            return ERROR;

        free(tmp);
    }
    else if (data->cur_ram_ind < RAM_SIZE) {

        tmp->place = VarPlaceRAM;
        tmp->var_code = data->cur_ram_ind++;

        if (pushBack(data->vars_data, tmp) != SUCCESS)
            return ERROR;

        free(tmp);
    }
    else {
        printf(RED "asm gen error: " END_OF_COLOR "unable to push variable\n");
        return ERROR;
    }

    return SUCCESS;

}

static void callMainPrint(FILE *fn) {

    assert(fn);

    fprintf(fn, "\t\tcall main\n");
    fprintf(fn, "\t\tpush %d\n\t\toutc\n", '\n');
    fprintf(fn, "\t\thlt\n");
}

static int getVarValue(size_t var_code, CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < data->vars_data->size; i++) {
        if (((Address *) getPtr(data->vars_data, i))->var_code == var_code) {
            if (((Address *) getPtr(data->vars_data, i))->place == VarPlaceRAM) {
                fprintf(data->fn, "\t\tpop rax\n\t\tpush %lu\n\t\tadd\n\t\tpop rbx\n\t\tpush [rbx]\n", ((Address *) getPtr(data->vars_data, i))->var_index);
            }
            else {
                fprintf(data->fn, "\t\tpush %s\n", REGS[((Address *) getPtr(data->vars_data, i))->var_index]);
            }
            return SUCCESS;
        }
    }

    printf(RED "gen asm error: " END_OF_COLOR "variable does not exist\n");

    return ERROR;
}

static int writeVarValue(size_t var_code, CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < data->vars_data->size; i++) {
        if (((Address *) getPtr(data->vars_data, i))->var_code == var_code) {
            fprintf(data->fn, "\t\tpush rax\n");
            fprintf(data->fn, "\t\tpush %lu\n\t\tadd\n\t\tpop rbx\n", var_code);
            if (((Address *) getPtr(data->vars_data, i))->place == VarPlaceRAM) {
                fprintf(data->fn, "\t\tpop [rbx]\n");
            }
            else {
                fprintf(data->fn, "\t\tpop %s\n", REGS[((Address *) getPtr(data->vars_data, i))->var_index]);
            }
            return SUCCESS;
        }
    }
    printf(RED "gen asm error: " END_OF_COLOR "variable to write value does not exist\n");

    return ERROR;
}

static int genLogicalJump(TreeNode *node, CodeGenData *data) {

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

static int genUnaryOp(TreeNode *node, CodeGenData *data) {

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

static int genBinaryOp(TreeNode *node, CodeGenData *data) {

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

static int dtorLocalVars(CodeGenData *data) {

    assert(data);

    size_t *index = (size_t *) calloc (1, sizeof(size_t));
    if (!index) return ERROR;
    if (pop(data->local_vars, index) != SUCCESS)
        return ERROR;

    Address *value = (Address *) calloc (1, sizeof(Address));
    if (!value) return ERROR;

    while (data->vars_data->size != *index)
        if (pop(data->vars_data, &value) != SUCCESS)
            return ERROR;

    free(index);
    free(value);

    return SUCCESS;
}

static VarSearchStatus ifVariableExists(size_t var_code, CodeGenData *data) {

    assert(data);

    for (size_t i = 0; i < data->vars_data->size; i++) {
        if (var_code == ((Address *) getPtr(data->vars_data, i))->var_code)
            return kStatusFound;
    }

    return kStatusNotFound;
}

static bool isPunct(TreeNode *node, Punctuation sym) {

    assert(node);

    return node->value.type == PUNCT_SYM && node->value.sym_code == sym;
}

static bool isBinOp(TreeNode *node, Binary_Op operation) {

    assert(node);

    return node->value.type == BINARY_OP && node->value.bin_op == operation;
}

static bool isUnOp(TreeNode *node, Unary_Op operation) {

    assert(node);

    return node->value.type == UNARY_OP && node->value.un_op == operation;
}

static bool isKeyOp(TreeNode *node, Key_Op operation) {

    assert(node);

    return node->value.type == KEY_OP && node->value.key_op == operation;
}

static bool isType(TreeNode *node, ValueType type) {

    assert(node);

    return node->value.type == type;
}