#include <stdlib.h>
#include <stdio.h>
#include "network.h"
#include "math.h"

////////////////////
// NETWORK MEMORY //
////////////////////

Network* create_node(void) {
    Network* node = malloc(sizeof(Network));

    if (node == NULL) {
        return NULL;
    }

    node -> id = -1;
    node -> left = NULL;
    node -> right = NULL;
    node -> parent = NULL;
    
    node -> left_length = 0;
    node -> right_length = 0;
    node -> parent_length = 0;
    node -> sink_capacitance = 0;

    node -> elmore_delay = 0;

    return node;
}

void network_destruct(Network* network) {
    if (network == NULL) {
        return;
    }

    network_destruct(network -> left);
    network_destruct(network -> right);
    free(network);
}

void print_network(Network* network) {
    if (network == NULL) {
        return;
    }

    printf("--- NODE %04d ", network -> id);
    
    switch (network -> type) {
        case ROOT: 
            printf(" ROOT");
            break;
        case INTERNAL: 
            printf("INNER");
            break;
        case LEAF: 
            printf(" LEAF");
            break;
    }

    printf(" ---\n");
    printf("C': %+.10le, Cs: %+.10le, ED: %+.10le\n", network -> node_capacitance, network -> seen_capacitance, network -> elmore_delay);
    printf("SELF   | Sink:   %+.10le, Addr: %p\n", network -> sink_capacitance, (void*) (network));
    printf("LEFT   | Length: %+.10le, Addr: %p\n", network -> left_length, (void*) (network -> left));
    printf("RIGHT  | Length: %+.10le, Addr: %p\n", network -> right_length, (void*) (network -> right));
    printf("PARENT | Length: %+.10le, Addr: %p\n", network -> parent_length, (void*) (network -> parent));

    print_network(network -> left);
    print_network(network -> right);
}

///////////////////
// NETWORK STACK //
///////////////////

Network_Stack* ns_push(Network_Stack* ns, Network* network) {
    Network_Stack* ns_new = malloc(sizeof(Network_Stack));
    if (ns_new == NULL) {
        return NULL;
    }

    ns_new -> next = ns;
    ns_new -> network = network;

    return ns_new;
}

Network_Stack* ns_pop(Network_Stack* ns, Network** network) {
    if (ns_is_empty(ns)) {
        *network = NULL;
        return ns;
    }
    
    Network_Stack* ns_new = ns -> next;
    *network = ns -> network;

    free(ns);
    return ns_new;
}

int ns_is_empty(Network_Stack* ns) {
    return ns == NULL;
}

void ns_destruct(Network_Stack* ns) {
    Network* _;
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &_);
        network_destruct(_);
    }
}

//////////////////
// CALCULATIONS //
//////////////////

void calculate_network_node_capacitaces(Network* network, GP gp) {
    Network_Stack* ns = NULL;
    Network* node;

    ns = ns_push(ns, network);
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &node);

        if (node != NULL) {
            node -> node_capacitance = calculate_node_capacitance(node, gp);
            ns = ns_push(ns, node -> right);
            ns = ns_push(ns, node -> left);
        }
    }
    ns_destruct(ns);

    network -> node_capacitance += gp.inv_c_out;
}

double calculate_node_capacitance(Network* node, GP gp) {
    return node -> sink_capacitance + gp.unit_c * (node -> parent_length + node -> left_length + node -> right_length) / 2;
}

// Rewrite w/o recursion
void calculate_network_seen_capacitaces(Network* network, GP gp) {
    _calculate_network_seen_capacitaces(network);
}

void _calculate_network_seen_capacitaces(Network* network) {
    if (network == NULL) {
        return;
    }

    network -> seen_capacitance = network -> node_capacitance;

    if (network -> left != NULL) {
        _calculate_network_seen_capacitaces(network -> left);
        network -> seen_capacitance += network -> left -> seen_capacitance;
    }

    if (network -> right != NULL) {
        _calculate_network_seen_capacitaces(network -> right);
        network -> seen_capacitance += network -> right -> seen_capacitance;
    }
}

void calculate_network_elmore_delays(Network* network, GP gp) {
    Network_Stack* ns = NULL;
    Network* node;

    ns = ns_push(ns, network);
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &node);

        if (node != NULL) {
            if (node -> parent == NULL) {
                node -> elmore_delay = gp.inv_r_out * (node -> seen_capacitance);
            }
            else {
                node -> elmore_delay = node -> parent -> elmore_delay +
                gp.unit_r * (node -> parent_length) * (node -> seen_capacitance);
            }
            ns = ns_push(ns, node -> right);
            ns = ns_push(ns, node -> left);
        }
    }
    ns_destruct(ns);
}

//////////////////////
// BUFFER INSERTION //
//////////////////////

void insert_buffers(Network* network, GP gp) {
    Network_Stack* ns = NULL;
    Network* node;

    ns = ns_push(ns, network);
    while (!ns_is_empty(ns)) {
        ns = ns_pop(ns, &node);

        if (node != NULL) {

            if (node -> elmore_delay > gp.constraint_delay) {
                
            };

            ns = ns_push(ns, node -> right);
            ns = ns_push(ns, node -> left);
        }
    }
    ns_destruct(ns);
}
