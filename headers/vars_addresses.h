#ifndef VARIABLES_ADDRESSES
#define VARIABLES_ADDRESSES

#include <stdio.h>

enum VarPlace {
    VarPlaceRAM,
    VarPlaceREGS,
};

struct Address {
    int var_code;
    VarPlace place;
    size_t var_index;
};

struct VarsAddresses {
    Address *data;
    size_t size;
    size_t capacity;
};

int VarsAddressesCtor(VarsAddresses *data);
int VarsAddressesDtor(VarsAddresses *data);
int PushVarAddress(VarsAddresses *data, Address value);
int PopVarsAddresses(VarsAddresses *data, Address *value);

#endif