#ifndef SYSTEM_WORDS_LANG
#define SYSTEM_WORDS_LANG

enum ValueType {
    PUNCT_SYM = 0,
    BINARY_OP = 1,
    UNARY_OP  = 2,
    NUMBER    = 3,
    VARIABLE  = 4,
    FUNCTION  = 5,
};

enum Punctuation {
    END_SYMBOL     = 0, // \0

    OP_PARENTHESIS = 1, // (
    CL_PARENTHESIS = 2, // )

    OP_BRACE       = 3, // {
    CL_BRACE       = 4, // }

    NEW_LINE       = 5, // ;

};

enum Binary_Op {
    ADD    = 0,
    SUB    = 1,
    MUL    = 2,
    DIV    = 3,
    POW    = 4,

    EQUAL  = 5,
    ABOVE  = 6,
    BELOW  = 7,
    BEL_EQ = 8,
    AB_EQ  = 9,
    NOT_EQ = 10,

    ASSIGN = 11,
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

    IF    = 8,
    WHILE = 9,
};

enum KeyWordIndex {
};

#endif