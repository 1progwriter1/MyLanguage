#ifndef PROGRAM_OUTPUT_FILE
#define PROGRAM_OUTPUT_FILE

#include "../bin_tree/bin_tree.h"
#include "../../MyLibraries/headers/my_vector.h"
#include "../headers/key_words_codes.h"
#include "lex_analysis.h"
#include "../lib_src/my_lan_lib.h"

int printInFile(TreeStruct *tree, Vector *names_table, const char *filename);

#endif