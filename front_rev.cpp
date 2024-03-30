#include <stdio.h>
#include "../MyLibraries/headers/systemdata.h"
#include "../MyLibraries/headers/file_func.h"
#include "headers/file_read_lang.h"
#include <assert.h>
#include "headers/gen_prog.h"
#include "headers/names_table.h"
#include "headers/prepare_front_rev.h"

int main(const int argc, const char *argv[]) {

    assert(argv);

    const char *src_file = SRC_FILE;
    const char *dst_file = DST_FILE;

    if (argc > 1) {
        src_file = argv[1];
        if (argc > 2)
            dst_file = argv[2];
    }

    TreeStruct tree = {};

    if (ReadFileLang(&tree, src_file) != SUCCESS) {
        printf(RED "frontend rev error: " END_OF_COLOR "file read failed\n");
        return ERROR;
    }

    FILE *fn = fileopen(src_file, READ);
    if (!fn) return FILE_OPEN_ERROR;

    NamesTable names = {};

    if (PrepareToProgGen(tree.root, fn, &names) == SUCCESS)
        return ERROR;

    return SUCCESS;
}