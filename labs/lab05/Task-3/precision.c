#include <stdio.h>

int main(int argc, char **argv)
{

    float epsilon = 1;
    int count;
    float result = 2;

    while (result > 1) {
        count++;
        epsilon *= 0.5;
        result = 1 + epsilon;
    }
    printf("last viable epsilon: 2^-%d\n", count);

    return 0;
}
