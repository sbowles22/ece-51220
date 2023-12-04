//****************************************************************************//
// FILE INPUT AND OUTPUT FUNCTIONS
//****************************************************************************//

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "file_io.h"

////////////////
// FILE INPUT //
////////////////

Graph* load_gt_from_file(char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }

    Graph* g = malloc(sizeof(Graph));
    if (g == NULL) {
        fclose(f);
        return NULL;
    }

    int V;
    int E;
    int elist_start;

    double weight;
    int src;
    int dest; 

    fscanf(f, "V %d\n", &V);
    fscanf(f, "E %d\n", &E);
    elist_start = ftell(f);

    g -> V = V;
    g -> E = E;
    
    g -> vlist = NULL;
    g -> cmem = NULL;
    g -> mcm_cycle = NULL;

    if (g_create_cmem(g) == -1) {
        g_destruct(g);
        fclose(f);
        return NULL;
    }

    g -> vlist = calloc(V, sizeof(Vertex));
    if (g -> vlist == NULL) {
        g_destruct(g);
        fclose(f);
        return NULL;
    }

    for (int i = 0; i < E; i++) {
        fscanf(f, "%d %d %lf\n", &dest, &src, &weight);
        (g -> cmem)[v_ix(src)] += 1; 
    }

    for (int i = 0; i < V; i++) {
        (g -> vlist)[i].id = v_id(i);
        (g -> vlist)[i].e = (g -> cmem)[i];
        (g -> vlist)[i].elist = calloc((g -> cmem)[i], sizeof(Edge));
        if ((g -> vlist)[i].elist == NULL) {
            g_destruct(g);
            fclose(f);
            return NULL;
        }
    }

    #ifdef DEBUG
        for (int i = 0; i < V; i++) {
            printf("%d ", (g -> cmem)[i]);
        }
        printf("\n");
    #endif

    fseek(f, elist_start, SEEK_SET);
    for (int i = 0; i < E; i++) {
        fscanf(f, "%d %d %lf\n", &dest, &src, &weight);
        ((g -> vlist)[v_ix(src)].elist)[(g -> cmem)[v_ix(src)] - 1].w = weight;
        ((g -> vlist)[v_ix(src)].elist)[(g -> cmem)[v_ix(src)] - 1].dest = v_ix(dest);
        (g -> cmem)[v_ix(src)] -= 1;
    }

    #ifdef DEBUG
        for (int i = 0; i < V; i++) {
            printf("%d ", (g -> cmem)[i]);
        }
        printf("\n");
        for (int i = 0; i < V; i++) {
            if ((g -> cmem)[i] != 0) {
                printf("ERROR: Edge count remaining in graph loading\n");
                g_destruct(g);
                fclose(f);
                return NULL;
            }
        }
    #endif
    
    fclose(f);
    return g;
}

/////////////////
// FILE OUTPUT //
/////////////////

int write_mcm_to_file(char* filename, Graph* g) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    float mcm = g -> min_cycle_mean; 

    fwrite(&mcm, sizeof(float), 1, f);

    fclose(f);
    return 0;
}

int write_mcm_cycle_to_file(char* filename, Graph* g) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    for (int i = 0; i < g -> mcm_cycle_len; i++) {
        fprintf(f, "%d%c", v_id((g -> mcm_cycle)[i]), (i == g -> mcm_cycle_len - 1) ? '\n' : ' ');
    }

    fclose(f);
    return 0;
}

/////////////////////
// FILE IO UTILITY //
/////////////////////

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}
