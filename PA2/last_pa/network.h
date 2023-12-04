#ifndef NETWORK_HEADER
#define NETWORK_HEADER

typedef enum {
    ROOT, INTERNAL, LEAF
} NodeType;

typedef struct _Tnode {
    int id;
    NodeType type;

    double node_capacitance;
    double seen_capacitance;
    double elmore_delay;

    double sink_capacitance;

    double left_length;
    struct _Tnode* left;

    double right_length;
    struct _Tnode* right;

    double parent_length;
    struct _Tnode* parent;
} Network;

typedef struct _Lnode {
    struct _Lnode* next;
    Network* network;
} Network_Stack;

typedef struct { // Global Network Parameters
    float constraint_delay;
    float inv_intrinsic_delay;
    double inv_c_in;
    double inv_c_out;
    double inv_r_out;
    double unit_c;
    double unit_r;
} GP;

Network* create_node(void);
void network_destruct(Network* network);
void print_network(Network* network);

Network_Stack* ns_push(Network_Stack* ns, Network* network);
Network_Stack* ns_pop(Network_Stack* ns, Network** network);
int ns_is_empty(Network_Stack* ns);
void ns_destruct(Network_Stack* ns);

// Calculations
void calculate_network_node_capacitaces(Network* network, GP gp);
double calculate_node_capacitance(Network* network, GP gp);
void calculate_network_seen_capacitaces(Network* network, GP gp);
void _calculate_network_seen_capacitaces(Network* network);
void calculate_network_elmore_delays(Network* network, GP gp);

// Buffer Insertion
void insert_buffers(Network* network, GP gp);

#endif
