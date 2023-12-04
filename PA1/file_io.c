#include <stdlib.h>
#include <stdio.h>
#include "network.h"
#include "file_io.h"

#define BUF_SIZE 256

int load_repeater_from_file(char* filename, double* inv_c_in, double* inv_c_out, double* inv_r_out) {
    FILE* f;

    f = fopen(filename, "r");
    if (f == NULL) {
        return -1;
    }

    if (fscanf(f, "%le %le %le\n", inv_c_in, inv_c_out, inv_r_out) != 3) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

int load_wire_from_file(char* filename, double* unit_c, double* unit_r) {
    FILE* f;

    f = fopen(filename, "r");
    if (f == NULL) {
        return -1;
    }

    if (fscanf(f, "%le %le\n", unit_r, unit_c) != 2) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

Network* load_network_from_file(char* filename) {
    Network* node;
    Network* l_net;
    Network* r_net;
    Network_Stack* ns = NULL;

    int id;
    double sink_cap;
    double wire_l;
    double wire_r;

    FILE* f;

    f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }

    while (!feof(f)) {
        node = create_node();
        if (node == NULL) {
            ns_destruct(ns);
            fclose(f);
            return NULL;
        }

        if (fpeek(f) != '(') { // LEAF NODE
            if (fscanf(f, "%d(%le)\n", &id, &sink_cap) != 2) {
                network_destruct(node);
                ns_destruct(ns);
                fclose(f);
                return NULL;
            }

            node -> id = id;
            node -> type = LEAF;
            node -> sink_capacitance = sink_cap;
        }
        else { // INT NODE
            if (fscanf(f, "(%le %le)\n", &wire_l, &wire_r) != 2) {
                network_destruct(node);
                ns_destruct(ns);
                fclose(f);
                return NULL;
            }

            node -> id = 0;
            node -> type = INTERNAL;
            node -> left_length = wire_l;
            node -> right_length = wire_r;

            r_net = NULL;
            l_net = NULL;

            ns = ns_pop(ns, &r_net);
            if (r_net == NULL) {
                network_destruct(node);
                ns_destruct(ns);
                fclose(f);
                return NULL;
            }

            ns = ns_pop(ns, &l_net);
            if (l_net == NULL) {
                network_destruct(r_net);
                network_destruct(node);
                ns_destruct(ns);
                fclose(f);
                return NULL;
            }

            node -> left = l_net;
            node -> left -> parent = node;
            node -> left -> parent_length = wire_l;
            node -> right = r_net;
            node -> right -> parent = node;
            node -> right -> parent_length = wire_r;
        }

        // if (node -> parent != NULL) {
        //     if (node -> parent -> left == node) {
        //         node -> parent_length = node -> parent -> left_length;
        //     }
        //     if (node -> parent -> right == node) {
        //         node -> parent_length = node -> parent -> right_length;
        //     }
        // }

        ns = ns_push(ns, node);
    }
    
    fclose(f);

    ns = ns_pop(ns, &node);
    
    if (!ns_is_empty(ns)) {
        network_destruct(node);
        ns_destruct(ns);
        return NULL;
    }

    ns_destruct(ns);

    node -> type = ROOT;
    return node;
}

//////////////////
// OUTPUT FILES //
//////////////////

int write_network_to_file(char* filename, Network* network) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    Network_Stack* ns = NULL;
    Network* node;

    ns = ns_push(ns, network);
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &node);

        if (node == NULL) {
            fclose(f);
            ns_destruct(ns);
            return -1;
        }

        if (node -> id == 0) {
            fprintf(f, "(%.10le %.10le)\n", node -> left_length, node -> right_length);
            ns = ns_push(ns, node -> right);
            ns = ns_push(ns, node -> left);
        }
        else {
            fprintf(f, "%d(%.10le)\n", node -> id, node -> sink_capacitance);
        }
    }

    fclose(f);
    ns_destruct(ns);

    return 0;
}

int write_elmore_delay_to_file(char* filename, Network* network) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    Network_Stack* ns = NULL;
    Network* node;

    ns = ns_push(ns, network);
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &node);

        if (node == NULL) {
            fclose(f);
            ns_destruct(ns);
            return -1;
        }

        switch (node -> type) {
            case ROOT:
            case INTERNAL:
                ns = ns_push(ns, node -> right);
                ns = ns_push(ns, node -> left);
                break;
            case LEAF:
                fwrite(&(node -> id), sizeof(node -> id), 1, f);
                fwrite(&(node -> elmore_delay), sizeof(node -> elmore_delay), 1, f);
                break;
        }
    }

    fclose(f);
    ns_destruct(ns);

    return 0;
}

//////////
// MISC //
//////////

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}
