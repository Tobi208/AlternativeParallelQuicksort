#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
static void shuffle(int* xs, const unsigned int N) {
    for (unsigned int i = 0; i < N - 1; i++)
        swap(xs + i, xs + (i + rand() / (RAND_MAX / (N - i) + 1)));
}

/**
 * Reverse an array
*/
static void reverse(int* xs, const unsigned int N) {
    for (unsigned int i = 0; i < N / 2; i++)
        swap(xs + i, xs + N - 1 - i);
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
    if (argc != 5) {
        printf("Usage: ./generator N path dist seed\n");
        printf("       N    - input size\n");
        printf("       path - directory to store file in\n");
        printf("       dist - 0 = random order, 1 = random ints, 2 = sorted, 3 = reversed\n");
        printf("       seed - for the random number generator, 0 = current time\n");
        exit(-1);
    }
    const int   N    = atoi(argv[1]);
    const char* path = argv[2];
    const char  dist = atoi(argv[3]);
    const int   seed = atoi(argv[4]);

    // seed RNG
    if (seed <= 0) srand(time(NULL));
    else           srand(seed);

    // generate data
    int* xs = (int*) malloc(N * sizeof(int));
    
    // random integers
    if (dist == 1) {
        for (int i = 0; i < N; i++)
            xs[i] = rand();
    // random order
    } else
        for (int i = 0; i < N; i++)
            xs[i] = i;
    if (dist == 0)
        shuffle(xs, N);
    // reversed order
    else if (dist == 3)
        reverse(xs, N);

    // write output
    write_file(xs, N, path);

    return 0;    
}