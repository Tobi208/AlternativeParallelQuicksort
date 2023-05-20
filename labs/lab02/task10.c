#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_array(int* xs, int n) {
    for(int i = 0; i < n; i++)
        printf("%d ", xs[i]);
    printf("\n");
}

int main() {

    int* arr;
    int n;

    scanf("%d", &n);
    arr = (int*) malloc(n * sizeof(int));

    srand(time(0)); // initialize the random number generator
    for(int i = 0; i < n; ++i)
        arr[i] = rand() % 100; // random number from 0 to 99

    print_array(arr, n);
    return 0;

}
