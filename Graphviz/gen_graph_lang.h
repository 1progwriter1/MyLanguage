#ifndef GEN_GRAPH_LANG
#define GEN_GRAPH_LANG

#include "../diff_func/headers/bin_tree.h"

#define GRAPH_FILE "graph.png"
#define GRAPH_SRC  "graph.dot"

int GenGraphDiff(const TreeStruct *tree, const char *filename);

#endif