#include <stdio.h>
#include <math.h>
#include "func.h"
#include <assert.h>
#include "systemdata.h"
#include "sys/stat.h"

const double COMPARE_PRECISION = 1e-9;

int randnum(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

int GetNumber(int *number) {

    assert(number);

    int correct = 0;
    do {
        correct = 1;
        if (scanf("%d", number) != 1) {
            printf(RED "Incorrect input. Try again:\n " END_OF_COLOR);
            buf_clear();
            correct = 0;
        }
    } while (!correct);

    return SUCCESS;
}

void buf_clear() {
    while (getchar() != '\n')
        ;
}

int min(const int a, const int b) {
    return a > b ? a : b;
}

int max(const int a, const int b) {
    return a >= b ? a : b;
}

bool IsEqual(const double a, const double b) {
    return fabs(a - b) < COMPARE_PRECISION;
}
