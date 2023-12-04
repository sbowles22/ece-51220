#include "utils.h"
#include "math.h"

int max(int a, int b) {
    return (a > b) ? a : b; 
}

int is_power_of_2(int n)
{
    return (ceil(log2(n)) == floor(log2(n)));
}
