#include <stdio.h>
#include "ctree.h"

#ifndef FILE_IO_HEADER
#define FILE_IO_HEADER

CTree* load_ct_from_file(char* filename);

int write_area_to_file(char* filename, Layout* lout);
int write_layout_to_file(char* filename, Layout* lout);
void _write_layout_to_file(FILE* f, Option* option, int x, int y);

int fpeek(FILE *stream);

#endif
