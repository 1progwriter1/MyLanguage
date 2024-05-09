DEF_OP (SIN, 0, "sin", 1,
        NEW_S(OP(MUL),
            NEW_S(OP(COS), NULL,
            C(R)),
        D(R)),

        sin(CALC(R) * M_PI / 180), "\\sin")

DEF_OP (COS, 1, "cos", 1,
        NEW_S(OP(MUL),
            NEW(NUM(-1), NULL, NULL),
            NEW_S(OP(MUL),
                NEW_S(OP(SIN), NULL,
                    C(R)),
                D(R))),

        cos(CALC(R) * M_PI / 180), "\\cos")

DEF_OP (SQRT, 2, "sqrt", 1,
        NEW_S(OP(MUL),
            NEW_S(OP(DIV),
                NEW(NUM(1), NULL, NULL),
                NEW_S(OP(MUL),
                    NEW(NUM(2), NULL, NULL),
                    NEW_S(OP(POW),
                        C(R),
                        NEW(NUM(2), NULL, NULL)))),
            D(R)),

        sqrt(CALC(R)), "\\sqrt")

DEF_OP (LN, 3, "ln", 1,
        NEW_S(OP(MUL),
            NEW_S(OP(DIV),
                NEW(NUM(1), NULL, NULL),
                C(R)),
            D(R)),

        log(CALC(R)), "\\ln")

DEF_OP (ADD, 10, "+", 2,
        NEW_S(OP(ADD),
            D(L),
            D(R)),

        CALC(L) + CALC(R), "+")

DEF_OP (SUB, 11, "-", 2,
        NEW_S(OP(SUB),
            D(L),
            D(R)),

        CALC(L) - CALC(R), "-")

DEF_OP (MUL, 12, "*", 2,
        NEW_S(OP(ADD),
            NEW_S(OP(MUL),
                D(L),
                C(R)),
            NEW_S(OP(MUL),
                C(L),
                D(R))),

        CALC(L) * CALC(R), "*")

DEF_OP (DIV, 13, "/", 2,
        NEW_S(OP(DIV),
            NEW_S(OP(SUB),
                NEW_S(OP(MUL),
                    D(L),
                    C(R)),
                NEW_S(OP(MUL),
                    C(L),
                    D(R))),
            NEW_S(OP(POW),
                C(R),
                NEW(NUM(2), NULL, NULL))),

        CALC(L) / CALC(R), "dfrac")


DEF_OP (POW, 14, "^", 2,
        IsNum(R) ? NEW_S(OP(MUL), NEW_S(OP(MUL), C(R), NEW_S(OP(POW), C(L), NEW_S(OP(SUB), C(R), NEW(NUM(1), NULL, NULL)))), D(L)) :
            NEW_S(OP(MUL),
                C(node),
                NEW_S(OP(ADD),
                    NEW_S(OP(MUL),
                        D(R),
                        NEW_S(OP(LN), NULL,
                            C(L))),
                    NEW_S(OP(MUL),
                        C(R),
                        NEW_S(OP(DIV),
                            D(L),
                            C(L))))),

        pow(CALC(L), CALC(R)), "^")
