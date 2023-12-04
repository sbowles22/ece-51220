
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main(int argc, char** argv) {
    int good = 1;

    FILE* f1 = fopen(argv[1], "r");
    FILE* f2 = fopen(argv[2], "r");
    int node1;
    int node2;
    double ed1;
    double ed2;
    while (!feof(f1)) {
        fread(&node1, sizeof(node1), 1, f1);
        fread(&ed1, sizeof(ed1), 1, f1);
        fread(&node2, sizeof(node2), 1, f2);
        fread(&ed2, sizeof(ed2), 1, f2);

        if (ed1 * 0.99 > ed2 || ed1 * 1.01 < ed2 || node1 != node2) {
            printf(">\n");
            printf("%d != %d\n", node1, node2);
            printf("%.10le != %.10le\n", ed1, ed2);
            good = 0;
        }
    }
    fclose(f1);
    fclose(f2);
    
    if (good) {
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }
}
