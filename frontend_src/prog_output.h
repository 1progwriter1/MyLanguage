#ifndef PROGRAM_OUTPUT_FILE
#define PROGRAM_OUTPUT_FILE

#include "../bin_tree/bin_tree.h"
#include "../headers/names_table.h"

int PrintInFile(TreeStruct *tree, NamesTable *data, const char *fileanme);

#define DST_FILE "prog.txt"

#endif