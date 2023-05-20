#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int check_prime(int x) {

    for(int i = 2; i <= floor(sqrt(x)); i++)
        if(x % i == 0)
            return 0;

    return x >= 2;
}

int main() {

    int n;
    int* xs;

    scanf("%d", &n);
    xs = (int *)malloc(n * sizeof(int));

    int num_primes = 0;
    int* primes = (int *) malloc(num_primes * sizeof(int));

    for(int i = 0; i < n; i++) {

        scanf("%d", &xs[i]);
        
        if(check_prime(xs[i])) {

            num_primes++;
            primes = (int*) realloc(primes, num_primes * sizeof(int));
            primes[num_primes - 1] = xs[i];
            xs[i] = 0;

        }
    }

    printf("Primes: ");
    for(int i = 0; i < num_primes; i++) {
        printf("%d ", primes[i]);
    }
    printf("\nNum Primes: %d\n", num_primes);

    free(xs);
    free(primes);

}
