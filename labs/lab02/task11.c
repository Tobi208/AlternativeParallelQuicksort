#include <stdio.h>
#include <stdlib.h>

int main() {

    int sum = 0;
    int i = 0;
    int n = 10;
    int* xs = (int*) malloc(n * sizeof(int));
    int input;

    while(scanf("%d", &input) != 0) { 
        if(i == n) {
            n += 10; 
            xs = (int *) realloc(xs, n * sizeof(int));
        }
        xs[i] = input; 
        sum += input;
        i++;
    }

    printf("Output: ");
    for(int i = 0; i < n; i++)
        printf("%d ", xs[i]); 
    printf("\nSum: %d\n", sum);

    free(xs);

}
