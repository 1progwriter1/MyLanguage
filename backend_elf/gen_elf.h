#ifndef GENERATE_ELF
#define GENERATE_ELF

#include "../lib_src/bin_tree.h"
#include "backend_elf_data.h"

int genElf(TreeStruct *tree, const char *output_file);
int prepareToGen(CodeGenData *data);
int writeInFile(CodeGenData *data, const char *filename);
void endGen(CodeGenData *data);

int createHeader(CodeGenData *data);

#endif