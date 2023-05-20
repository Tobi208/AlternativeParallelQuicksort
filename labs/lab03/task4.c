#include <stdio.h>
#include <stdlib.h>

int CmpDouble(const void* a, const void* b) {
    if (&a < &b)
        return -1;
    else if (&a > &b)
        return 1;
    else
        return 0;
}

int main(int argc, char** argv) {

    double arrDouble[] = {9.3, -2.3, 1.2, -0.4, 2, 9.2, 1, 2.1, 0, -9.2};
    int arrlen = 10;

    qsort(arrDouble, arrlen, sizeof(double), CmpDouble);

    for (int i = 0; i < arrlen; i++) {
        printf("%f ", arrDouble[i]);
    }
    printf("\n");

    return 0;
}
