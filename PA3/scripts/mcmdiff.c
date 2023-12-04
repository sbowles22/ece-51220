#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "math.h"

#define NONZERO_TOLERANCE 1e-6
#define ZERO_TOLERANCE 1e-15

int main(int argc, char** argv) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }

    FILE* fp1 = fopen(argv[1], "r");
    FILE* fp2 = fopen(argv[2], "r");

    if (fp1 == NULL || fp2 == NULL) {
        return EXIT_FAILURE;
    }

    float mcm1;
    float mcm2;

    fread(&mcm1, sizeof(float), 1, fp1);
    fread(&mcm2, sizeof(float), 1, fp2);

    if (-ZERO_TOLERANCE <= mcm1 && mcm1 <= ZERO_TOLERANCE) {
        if (-ZERO_TOLERANCE <= mcm2 && mcm2 <= ZERO_TOLERANCE) {
            return EXIT_SUCCESS;
        }
    }

    if (fabs((mcm2 - mcm1) / mcm2) <= NONZERO_TOLERANCE) {
        return EXIT_SUCCESS;
    }

    printf("%e != %e\n", mcm1, mcm2);
    return EXIT_FAILURE;
}
