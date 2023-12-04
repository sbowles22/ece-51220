#include "network.h"

#ifndef FILE_IO_HEADER
#define FILE_IO_HEADER

int load_repeater_from_file(char* filename, double* inv_c_in, double* inv_c_out, double* inv_r_out);
int load_wire_from_file(char* filename,     double* unit_c,   double* unit_r);
Network* load_network_from_file(char* filename);

int write_network_to_file(char* filename, Network* network);
int write_elmore_delay_to_file(char* filename, Network* network);

int fpeek(FILE *stream);

#endif
