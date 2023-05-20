#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #define NDEBUG
#include <assert.h>

int is_prime(int n){
    assert(n > 2 && "n is greater than 2");
    for (int i = 2; i <= sqrt(n); i++)
        if (n % i == 0)
	    return 0;
    return 1;
}

int main(){
    
    int n, nprimes;
    int *prime;
    
    printf("Give a number: ");
    scanf("%d", &n);
    prime= (int *) malloc(n * sizeof(int));

    printf("%s: %d n=%d\n",__FILE__,__LINE__,n);
    
    // Compute all primes
    prime[0] = 2;
    nprimes = 1;
    for (int number = 3; number <= n; number+=2)
        if (is_prime(number)){
            prime[nprimes] = number;
            nprimes++;
        }
    
    
    if (nprimes < 1000)
    	for (int i = 0; i < nprimes; i++)
    		printf("%d\n",prime[i]);

    printf("Total number of primes: %d\n",nprimes);
    return 0;
}

