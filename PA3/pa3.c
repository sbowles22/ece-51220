#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "graph.h"
#include "file_io.h"

#ifdef PARALLEL
#include <omp.h>
#endif

int main(int argc, char** argv) {
    if (argc != 4) {
        return EXIT_FAILURE;
    }

#ifdef PARALLEL
    omp_set_num_threads(8);
#endif

    ////////////////
    // FIle Input //
    ////////////////

    Graph* g;
    g = load_gt_from_file(argv[1]);
    if (g == NULL) {
        g_destruct(g);
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        g_print(g);
    #endif

    ////////////////////
    // Min Cycle Mean //
    ////////////////////

    g_calc_mcm(g);

    if (write_mcm_to_file(argv[2], g) < 0) {
        g_destruct(g);
        return EXIT_FAILURE;
    }

    if (write_mcm_cycle_to_file(argv[3], g) < 0) {
        g_destruct(g);
        return EXIT_FAILURE;
    }
    
    g_destruct(g);
    return EXIT_SUCCESS;
}
