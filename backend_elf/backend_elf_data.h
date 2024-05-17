#ifndef BACKEND_ELF_DATA
#define BACKEND_ELF_DATA

#include <stdio.h>
#include "../../MyLibraries/headers/my_vector.h"

struct CodeGenData {
    Vector *code;
    char *tmp;
};

#endif