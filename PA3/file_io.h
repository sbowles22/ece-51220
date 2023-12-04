#include <stdio.h>
#include "graph.h"

#ifndef FILE_IO_HEADER
#define FILE_IO_HEADER

// File Input
Graph* load_gt_from_file(char* filename);

// File Output
int write_mcm_to_file(char* filename, Graph* g);
int write_mcm_cycle_to_file(char* filename, Graph* g);

int fpeek(FILE *stream);

#endif
