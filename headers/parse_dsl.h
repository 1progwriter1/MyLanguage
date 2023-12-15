#ifndef PARSING_DSL
#define PARSING_DSL

#define PAR_ASSERT              if (!IsPunct(data->position, CL_PARENTHESIS)) {                                 \
                                                printf(RED "Syntax error: " END_OF_COLOR "\nexpexted: )\n");    \
                                                data->error = CL_PARENTHESIS_MISSED;                            \
                                                return NULL;}                                                   \
//TODO: reduce
#define IsBinOp(position, operation) ((data->tokens->data[position].type == BINARY_OP) && (data->tokens->data[position].bin_op == operation))

#define IsUnOp(position, operation)  ((data->tokens->data[position].type == UNARY_OP) && (data->tokens->data[position].un_op == operation))

#define IsPunct(position, symbol)    ((data->tokens->data[position].type == PUNCT_SYM) && (data->tokens->data[position].sym_code == symbol))

#define IsEndSym(position)           ((data.tokens->data[position].type == PUNCT_SYM) && (data.tokens->data[position].sym_code == END_SYMBOL))

#define IsNumber(position)           (data->tokens->data[position].type == NUMBER)

#endif