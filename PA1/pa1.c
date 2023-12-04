#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "network.h"
#include "file_io.h"

int main(int argc, char** argv) {
    GP gp;
    Network* network;

    if (argc != 9) {
        return EXIT_FAILURE;
    }

    gp.constraint_delay = atof(argv[1]);
    if (gp.constraint_delay == 0.0) {
        return EXIT_FAILURE;
    }
    
    if (load_repeater_from_file(argv[2], &(gp.inv_c_in), &(gp.inv_c_out), &(gp.inv_r_out)) != 0) {
        return EXIT_FAILURE;
    }

    if (load_wire_from_file(argv[3], &(gp.unit_c), &(gp.unit_r)) != 0) {
        return EXIT_FAILURE;
    }

    network = load_network_from_file(argv[4]);
    if (network == NULL) {
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        printf("T:      %.10e\n", gp.constraint_delay);
        printf("C_IN:   %.10le, C_OUT:  %.10le, R_OUT: %.10le\n", gp.inv_c_in, gp.inv_c_out, gp.inv_r_out);
        printf("UNIT_C: %.10le, UNIT_R: %.10le\n", gp.unit_c, gp.unit_r);
    #endif

    if (write_network_to_file(argv[5], network) < 0) {
        network_destruct(network);
        return EXIT_FAILURE;
    }

    calculate_network_node_capacitaces(network, gp);
    calculate_network_seen_capacitaces(network, gp);
    calculate_network_elmore_delays(network, gp);

    if (write_elmore_delay_to_file(argv[6], network) < 0) {
        network_destruct(network);
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        print_network(network);
    #endif

    gp.inv_intrinsic_delay = gp.inv_c_out * gp.inv_r_out;
    if (gp.inv_intrinsic_delay > gp.constraint_delay) {
        return EXIT_FAILURE;
    }

    network_destruct(network);
    return EXIT_SUCCESS;
}
