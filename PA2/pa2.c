#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "layout.h"
#include "ctree.h"
#include "file_io.h"

int main(int argc, char** argv) {
    if (argc != 6) {
        return EXIT_FAILURE;
    }

    Layout* layout = malloc(sizeof(Layout));
    if (layout == NULL) {
        return EXIT_FAILURE;
    }

    ////////////////
    // FIle Input //
    ////////////////

    layout -> ct = load_ct_from_file(argv[1]);
    if (layout -> ct == NULL) {
        layout_destruct(layout);
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        printf("Pre layout:\n");
        ct_print(layout -> ct);
    #endif

    //////////////////
    // First Layout //
    //////////////////

    layout_no_choice(layout);

    #ifdef DEBUG
        printf("One option layout:\n");
        ct_print(layout -> ct);
    #endif

    if (write_area_to_file(argv[2], layout) < 0) {
        layout_destruct(layout);
        return EXIT_FAILURE;
    }

    if (write_layout_to_file(argv[3], layout) < 0) {
        layout_destruct(layout);
        return EXIT_FAILURE;
    }

    ///////////////////
    // Second Layout //
    ///////////////////

    layout_with_choice(layout);

    #ifdef DEBUG
        printf("All options layout:\n");
        ct_print(layout -> ct);
    #endif
    
    if (write_area_to_file(argv[4], layout) < 0) {
        layout_destruct(layout);
        return EXIT_FAILURE;
    }

    if (write_layout_to_file(argv[5], layout) < 0) {
        layout_destruct(layout);
        return EXIT_FAILURE;
    }

    layout_destruct(layout);
    return EXIT_SUCCESS;
}
