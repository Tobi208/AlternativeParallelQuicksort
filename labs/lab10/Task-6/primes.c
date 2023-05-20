#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {

  const int M = atoi(argv[1]);
  const int N = atoi(argv[2]);
  
  int num_primes = 0;

#pragma omp parallel firstprivate(M, N) reduction(+:num_primes) num_threads(N)
{
  int thread_num = omp_get_thread_num();
  int is_prime = 1;
  int local_primes = 0;

  for (int i = thread_num + 2; i <= M; i += N) {
    is_prime = 1;
    for (int j = 2; j < i; j++) {
      if (i % j == 0) {
        is_prime = 0;
        break;
      }
    }
    local_primes += is_prime;
  }

  num_primes += local_primes;
}

  printf("There are %d primes from 2 to %d\n", num_primes, M);

  return 0;
}
