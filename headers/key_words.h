#ifndef SYSTEM_WORDS_LANG
#define SYSTEM_WORDS_LANG

enum ValueType {
    NO_TYPE   = -1,
    PUNCT_SYM =  0,
    BINARY_OP =  1,
    UNARY_OP  =  2,
    KEY_OP    =  3,
    NUMBER    =  4,
    VARIABLE  =  5,
    FUNCTION  =  6,
    STRING    =  7,
};

enum Unary_Op {
    SIN   = 0,
    COS   = 1,
    SQRT  = 2,
    LN    = 3,

    NOT   = 4,

    OUT   = 5,
    OUT_S = 6,
    IN    = 7,

    CALL  = 8,
    RET   = 9,
};

enum Binary_Op {
    ADD    = 10,
    SUB    = 11,
    MUL    = 12,
    DIV    = 13,
    POW    = 14,

    EQUAL  = 15,
    ABOVE  = 16,
    BELOW  = 17,
    AB_EQ  = 18,
    BEl_EQ = 19,
    NOT_EQ = 20,

    ASSIGN = 21,
};

enum Key_Op {
    IF    = 22,
    WHILE = 23,
};

enum Punctuation {
    NEW_LINE       = 24, // ;

    END_SYMBOL     = 25, // \0

    OP_PARENTHESIS = 26, // (
    CL_PARENTHESIS = 27, // )

    OP_BRACE       = 28, // {
    CL_BRACE       = 29, // }
};

const size_t NUMBER_OF_KEY_WORDS = 29;

enum KeyWordIndex {
    UNARY_OP_INDEX     = 0,
    BINARY_OP_INDEX    = 10,
    KEY_OP_INDEX       = 22,
    FUNCTION_DEF_INDEX = 24,
    ADD_WORDS_INDEX    = 26,
};

extern const char *KEY_WORDS[NUMBER_OF_KEY_WORDS];

#endif