#ifndef GEN_GRAPH_LANG
#define GEN_GRAPH_LANG

#include "gen_graph_lang.h"
#include "../diff_func/headers/bin_tree.h"

#define GRAPH_FILE "Graphviz/graph.png"
#define GRAPH_SRC  "Graphvizgraph.dot"

int GenGraphLang(const TreeStruct *tree, const char *filename);

#endif