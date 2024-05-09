#ifndef DIFFERENTIATOR_DSL
#define DIFFERENTIATOR_DSL


#define CALC(node) CalculateNode(node, error)

#define PUNCT(sym)   (Token) {PUNCT_SYM, {.sym_code = sym}}

#define KEY_OP(oper) (Token) {KEY_OP, {.key_op = oper}}

#define UN_OP(oper)  (Token) {UNARY_OP, {.un_op = oper}}

#define BIN_OP(oper) (Token) {BINARY_OP, {.bin_op = oper}}

#define STR(str)     (Token) {STRING, {.string = str}}

#define FUNC(code)   (Token) {FUNCTION, {.func_index = code}}

#define NUM(num)     (Token) {NUMBER, {.number = num}}

#define VAR(ind)     (Token) {VARIABLE, {.var_index = ind}}

#define NEW(val, left, right)   treeNodeNew(tree, val ,left, right)

#define NEW_S(val, left, right) treeNodeNewSafe(tree, val ,left, right)

#define D(node) TakeNodeDerivative(tree, node, var_index)

#define C(node)  NodeCopy(tree, node)

#define L     node->left

#define R     node->right

#define LEFT  node->left

#define RIGHT node->right

#define IsValType(node, val_type)   (node->value.type == val_type)

#define IsUnaryOp(node, operation)    (IsValType(node, UNARY_OP) && node->value.un_op == operation)

#define IsBinaryOp(node, operation)   (IsValType(node, BINARY_OP) && node->value.bin_op  == operation)

#define IsZero(node)  ( IsValType(node, NUMBER) && IsEqual(node->value.number, 0) )

#define IsOne(node)   ( IsValType(node, NUMBER) && IsEqual(node->value.number, 1) )

#define EQ(node, num) ( IsValType(node, NUMBER) && IsEqual(node->value.number, num) )

#endif