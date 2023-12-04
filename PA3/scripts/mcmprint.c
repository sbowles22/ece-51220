#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "math.h"

#define NONZERO_TOLERANCE 1e-6
#define ZERO_TOLERANCE 1e-15

int main(int argc, char** argv) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    FILE* fp1 = fopen(argv[1], "r");

    if (fp1 == NULL) {
        return EXIT_FAILURE;
    }

    float mcm1;

    fread(&mcm1, sizeof(float), 1, fp1);

    printf("%f\n", mcm1);
    return EXIT_SUCCESS;
}
