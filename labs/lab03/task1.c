#include <stdio.h>
#include <stdlib.h>

#define N 5

int main(int argc, char** argv) {

    int matrix[N][N];

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i > j)
                matrix[i][j] = -1;
            else if (i < j)
                matrix[i][j] = 1;
            else
                matrix[i][j] = 0;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%2d ", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}