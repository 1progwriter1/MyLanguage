#ifndef GEN_GRAPH_LANG
#define GEN_GRAPH_LANG

#include "gen_graph_lang.h"
#include "../bin_tree/bin_tree.h"
#include "../frontend_src/lex_analysis.h"
#include "../../MyLibraries/headers/my_vector.h"

#define GRAPH_FILE "Graphviz/graph.png"
#define GRAPH_SRC  "Graphviz/graph.dot"

int genGraphLang(const TreeStruct *tree, const char *filename, Vector *names_table);

#endif