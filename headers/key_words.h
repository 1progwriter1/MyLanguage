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

    CALL    = 8,
    RET = 9,
};

enum Key_Op {
    IF    = 0,
    WHILE = 1,
};

const size_t NUMBER_OF_KEY_WORDS = 29;

enum KeyWordIndex {
    FUNCTION_DEF_INDEX = 0,
    BINARY_OP_INDEX    = 2,
    UNARY_OP_INDEX     = 14,
    KEY_OP_INDEX       = 24,
    ADD_WORDS_INDEX    = 26,
};

static const char *KEY_WORDS[NUMBER_OF_KEY_WORDS] = {   "Once upon a time", "fairytale",                // main, function
                                                        "+", "-", "*", "/", "^", "turned into",         // +, -, *, /, ^, ==
                                                        "stronger", "weeker", "not stronger",           // >, <, <=
                                                        "not weeker", "did not turn into", "turn into", // >=, != =
                                                        "sin", "cos", "sqrt", "ln", "not",              // sin, cos, sqrt, ln, !
                                                        "say the magic number", "say the magic word",   // printf("%lg"), printf("%s")
                                                        "guess the riddle", "end",                      // scanf("%lg"), return from function
                                                        "and they lived happily ever after",            // return from main
                                                        "stone", "fell into a dead sleep while",        // if, while
                                                        "go left", "go right", "fairytale character"    // words to make code be like a fairy tale
                                                    };

#endif