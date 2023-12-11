#ifndef PARSING_DSL
#define PARSING_DSL

#define PAR_ASSERT              if (!IsClBracket(data->position)) {                                             \
                                                printf(RED "Syntax error: " END_OF_COLOR "\nexpexted: )\n");    \
                                                data->error = CL_PARENTHESIS_MISSED;                            \
                                                return 0;}                                                      \
//TODO: reduce
#define IsAdd(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].operation == TOKEN_ADD))

#define IsSub(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].operation == TOKEN_SUB))

#define IsDiv(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].operation == TOKEN_DIV))

#define IsMul(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].operation == TOKEN_MUL))

#define IsCos(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].sys_word == SW_COS))

#define IsSin(position)         ((data->tokens->data[position].type == TOKEN_OPERATION) && (data->tokens->data[position].sys_word == SW_SIN))

#define IsClBracket(position)   (data->tokens->data[position].type == TOKEN_CL_BRACKET)

#define IsOpBracket(position)   (data->tokens->data[position].type == TOKEN_OP_BRACKET)

#define IsNumber(position)       (data->tokens->data[position].type == TOKEN_NUMBER)

#endif