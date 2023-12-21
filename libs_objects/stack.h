#ifndef STACK_STACK
#define STACK_STACK

#include "stack_values.h"

#define dump(stk, error) StackDump(error, (const char *)__FILE__, __LINE__, (const char *)__func__, stk);

int StackCtor(Stack *stk, const char *name, const char *file, const int line, const char *func);
void StackDump(unsigned int error, const char *file, const int line, const char *func, const Stack *stk);
int StackPop(Stack *stk, Elem_t *n);
int StackPush(Stack *stk, Elem_t n);
int StackDtor(Stack *stk);
unsigned int StackVerify(const Stack *stk);
void PrintStack(const Stack *stk);
void Detor();

#endif
