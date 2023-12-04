#ifndef FILE_GRAPH_HEADER
#define FILE_GRAPH_HEADER

#define MCM_TOLERANCE 1e-6
#define MCM_ZERO_TOLERANCE 1e-15

typedef struct _v {
    int id;
    int e;

    struct _e* elist;
} Vertex;

typedef struct _e {
    double w;
    int dest;
} Edge;

typedef struct _g {
    int V;
    int E;

    Vertex* vlist;

    int* cmem; // count memory

    // MCM
    float min_cycle_mean;
    int mcm_cycle_len;
    int* mcm_cycle;
} Graph;

void g_destruct(Graph* g);
void g_print(Graph* g);

// Count memory functions
int g_create_cmem(Graph* g);
void g_reset_cmem(Graph* g);

// Distance Predecesor Struct
typedef struct {
    float d;
    int p;
} DP;

// Minimum Cycle Mean Functions
int g_calc_mcm(Graph* g);
void g_calc_mcm_k(Graph* g, DP** dp, double* lks, int k, double* lk);
double g_calc_mcm_from_cycle(Graph* g);
int g_all_cycles_k(Graph* g, DP** dp, int k);
int g_pi_holds(Graph* g, DP** dp, double* lks, int k, double lk);

int v_id(int i);
int v_ix(int i);

void matrix_destruct(void** ptr, int n);

#endif
