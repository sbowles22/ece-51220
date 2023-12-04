#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "graph.h"
#include "utils.h"
#include "math.h"

#ifdef PARALLEL
#include <omp.h>
#endif

/////////////////////
// GRAPH FUNCTIONS //
/////////////////////

void g_destruct(Graph* g) {
    if (g == NULL) {
        return;
    }

    int v = g -> V;
    Vertex* vlist = g -> vlist;

    if (vlist != NULL) {
        for (int i = 0; i < v; i++) {
            if (vlist[i].elist != NULL) {
                free(vlist[i].elist);
            }
        }
        free(vlist);
    }

    if (g -> cmem != NULL) {
        free(g -> cmem);
    }
    
    if (g -> mcm_cycle != NULL) {
        free(g -> mcm_cycle);
    }

    free(g);
}

void g_print(Graph* g) {
    if (g == NULL) {
        printf("NULL GRAPH\n");
        return;
    }

    printf("|V| = %d, |E| = %d\n", g -> V, g -> E);

    Vertex* vlist = g -> vlist;
    Vertex v;

    printf("  ID  |  #E  | Dest\n");
    printf("===================\n");
    for (int i = 0; i < g -> V; i++) {
        v = vlist[i];
        printf(" %4d | %4d | ", v.id, v.e);
        for (int j = 0; j < v.e; j++) {
            // printf("%p, ", (void*)(v.elist)[j].dest);
            printf("%d%c ", v_id((v.elist)[j].dest), (j == v.e - 1) ? ' ' : ',');
        }
        printf("\n");
    }
}

// Create Counting Memory Function
// 0  Success
// -1 Error
int g_create_cmem(Graph* g) {
    g -> cmem = calloc(g -> V, sizeof(int));
    if (g -> cmem == NULL) {
        return -1;
    }
    
    return 0;
}

// Reseting counting memory Function
void g_reset_cmem(Graph* g) {
    for (int i = 0; i < g -> V; i++) {
        (g -> cmem)[i] = 0;
    }
}

////////////////////////
// MINIMUM CYCLE MEAN //
////////////////////////

int g_calc_mcm(Graph* g) {
    int V = g -> V;
    Vertex* vlist = g -> vlist;

    double lk;
    double* lks = malloc(V * sizeof(double));
    if (lks == NULL) {
        return -1;
    }

    DP** dp = calloc(V, sizeof(DP*));
    if (dp == NULL) {
        free(lks);
        return -1;
    }

    dp[0] = malloc(V * sizeof(DP));
    if (dp[0] == NULL) {
        free(lks);
        matrix_destruct((void**) dp, V);
        return -1;
    }
    for (int i = 0; i < V; i++) {
        dp[0][i].d = INFINITY;
        dp[0][i].p = -1;
    }

    dp[0][0].d = 0;

    for (int k = 0; k < V - 1; k++) {
        dp[k + 1] = malloc(V * sizeof(DP));
        if (dp[k + 1] == NULL) {
            free(lks);
            matrix_destruct((void**) dp, V);
            return -1;
        }
        for (int i = 0; i < V; i++) {
            dp[k + 1][i].d = INFINITY;
            dp[k + 1][i].p = -1;
        }

        dp[0][0].d = 0;

        #ifdef PARALLEL
        #pragma omp parallel for schedule(dynamic, 50)
        #endif
        for (int i = 0; i < V; i++) {
            if (isfinite(dp[k][i].d)) {
                for (int j = 0; j < vlist[i].e; j++) {
                    Edge e = vlist[i].elist[j];
                    double d = dp[k][i].d + e.w;
                    if (dp[k + 1][e.dest].d > d) {
                        dp[k + 1][e.dest].d = d;
                        dp[k + 1][e.dest].p = i;
                    }
                }
            }
        }

        // Early Termination
        if (k != 0 && is_power_of_2(k) && g_all_cycles_k(g, dp, k)) {
            // printf("EARLY TERM CHANCE @ %d\n", k);
            g_calc_mcm_k(g, dp, lks, k, &lk);
            if (g_pi_holds(g, dp, lks, k, lk)) {
                printf("EARLY TERM @ %d\n", k);
                g -> min_cycle_mean = lk;
                free(lks);
                matrix_destruct((void**) dp, V);
                return 0;
            }
        }
    }

#ifdef DEBUG
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            printf("%+5.1lf ", dp[i][j].d);
        }
        printf("\n");
    }
#endif

    g_calc_mcm_k(g, dp, lks, V - 1, &lk); 

    g -> min_cycle_mean = lk;

    free(lks);
    matrix_destruct((void**) dp, V);
    return 0;
}

void g_calc_mcm_k(Graph* g, DP** dp, double* lks, int k, double* plk) {
    if (k == 0) {
        return;
    }

    int V = g -> V;
    for (int i = 0; i < V; i++) {
        lks[i] = -INFINITY;
        if (isfinite(dp[k][i].d)) {    
            for (int n = 0; n < k - 1; n++) {
                if (isfinite(dp[n][i].d)) {
                    double karp = (dp[k][i].d - dp[n][i].d) / (k - n);
                    if (karp > lks[i]) {
                        lks[i] = karp;
                    }
                }
            }
        }
    }

    int vend = -1; 
    *plk = INFINITY;
    for (int i = 0; i < V; i++) {
        if (isfinite(lks[i]) && lks[i] < *plk) {
            *plk = lks[i];
            vend = i;
        }
    }

    // MCM Cycle Traceback
    int vlook = vend;
    int n;
    g_reset_cmem(g);

    for (n = k; (g -> cmem)[vlook] == 0; n--) {
        (g -> cmem)[vlook] = n;
        vlook = dp[n][vlook].p;
    }

    vend = vlook;
    g -> mcm_cycle_len = (g -> cmem)[vlook] - n;
    if (g -> mcm_cycle != NULL) {
        free(g -> mcm_cycle);
    }
    g -> mcm_cycle = malloc(g -> mcm_cycle_len * sizeof(int));

    vlook = vend;
    for (int i = 0; i < g -> mcm_cycle_len; i++) {
        (g -> mcm_cycle)[i] = vlook;
        vlook = dp[(g -> cmem)[vend] - i][vlook].p;
    }

    *plk = g_calc_mcm_from_cycle(g);
}

// Calculates mcm directly from the min cycle
double g_calc_mcm_from_cycle(Graph* g) {
    double mcm = 0.0;
    for (int i = 0; i < g -> mcm_cycle_len; i++) {
        int s = (g -> mcm_cycle)[i];
        Vertex v = (g -> vlist)[s];
        int t = (g -> mcm_cycle)[(i == 0) ? (g -> mcm_cycle_len) - 1 : (i-1)];
        for (int j = 0; j < v.e; j++) {
            if (v.elist[j].dest == t) {
                mcm += v.elist[j].w;
                break;
            }
        }
    }
    
    return mcm / g -> mcm_cycle_len;
}

// 0 Not finite Dk all are cycles
// 1 All finite Dk are cycles
int g_all_cycles_k(Graph* g, DP** dp, int k) {
    int* cmem = g -> cmem;
    g_reset_cmem(g);

    for (int i = 0; i < g -> V; i++) {
        if (isfinite(dp[k][i].d)) {
            int vlook = i;
            for (int n = k; cmem[vlook] != i + 1; n--) {
                if (n < 1) {
                    return 0;
                }
                cmem[vlook] = i + 1;
                vlook = dp[n][vlook].p;
            }
        }
    }

    return 1;
}

int g_pi_holds(Graph* g, DP** dp, double* lks, int k, double lk) {
    double* pi = malloc(g -> V * sizeof(double));
    if (pi == NULL) {
        return 0;
    }

    for (int i = 0; i < g -> V; i++) {
        pi[i] = INFINITY;
        for (int j = 0; j <= k; j++) {
            if (isfinite(dp[j][i].d)) {
                double pi_j = dp[j][i].d - j*lk;
                if (pi_j < pi[i]) {
                    pi[i] = pi_j;
                }
            }
        }
    }

    // for (int i = 0; i < g -> V; i++) {
    //     printf("%lf ", pi[i]);
    // }
    // printf("\n");


    double lstar = INFINITY;
    for (int i = 0; i < g -> V; i++) {
        int src = i;
        Vertex* vlist = g -> vlist;
        for (int j = 0; j < vlist[i].e; j++) {
            int dest = vlist[i].elist[j].dest;
            if (isfinite(pi[src])) {
                if (lstar > pi[src] - pi[dest] + vlist[i].elist[j].w) {
                    lstar = pi[src] - pi[dest] + vlist[i].elist[j].w;
                }
            }
        }
    }

    free(pi);

    if (fabs(lstar) * (1.0 - MCM_TOLERANCE) < fabs(lk)) {
        return 1;
    }

    return 0;
}

//////////////////////
// VERTEX FUNCTIONS //
//////////////////////

int v_id(int ix) {
    return ix + 1;
}

int v_ix(int id) {
    return id - 1;
}

//////////////////////
// MATRIX FUNCTIONS //
//////////////////////

void matrix_destruct(void** ptr, int n) {
    if (ptr == NULL) {
        return;
    }

    for (int i = 0; i < n; i++) {
        if (ptr[i] != NULL) {
            free(ptr[i]);
        }
    }

    free(ptr);
}
