#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Swap two integer values
 * as taken from the lecture
*/
static void swap(int* x, int* y) {
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

/**
 * Fisher-Yates shuffle
 * https://stackoverflow.com/questions/6127503/shuffle-array-in-c
*/
static void shuffle(int* xs, const int N) {
    for (int i = 0; i < N - 1; i++)
        swap(&xs[i], &xs[i + rand() / (RAND_MAX / (N - i) + 1)]);
}

/**
 * Write array of integers to file
 * as taken from the lecture 
*/
static void write_file(const int* xs, const int N, const char* path) {
    
    // unsafe string formatting
    // https://stackoverflow.com/questions/804288/creating-c-formatted-strings-not-printing-them
    char filename[64];
    sprintf(filename, "%s/%d", path, N);

    FILE* file = fopen(filename, "w");
    fwrite(xs, sizeof(int), N, file);
    fclose(file);
}

int main(int argc, char** argv) {

    // parse input
    if (argc != 3) {
        printf("Usage: ./generator N (input size) path (directory to store file in)\n");
        exit(-1);
    }
    const int N = atoi(argv[1]);
    const char* path = argv[2];

    // generate data
    int* xs = (int*) malloc(N * sizeof(int));
    for (int i = 0; i < N; i++)
        xs[i] = i + 1;
    shuffle(xs, N);

    // write output
    write_file(xs, N, path);

    return 0;    
}