#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct args {
  int start;
  int end;
  int* result;
} args;

void* thread_worker(void* thread_args) {
  args* targs = (args*) thread_args;
  int start = targs->start;
  int end = targs->end;
  int* result = targs->result;

  int num_primes = 0;
  int is_prime = 1;

  for (int i = start; i <= end; i++) {
    is_prime = 1;
    for (int j = 2; j < i; j++) {
      if (i % j == 0) {
        is_prime = 0;
        break;
      }
    }
    num_primes += is_prime;
  }
  * (int*) result = num_primes;
  return NULL;
}

int main(int argc, char** argv) {

  const int M = atoi(argv[1]);

  /* Start thread. */
  pthread_t thread1;
  pthread_t thread2;

  int sum1 = 0;
  int sum2 = 0;
  args thread1_args = {2, M / 2, &sum1};
  args thread2_args = {M / 2 + 1, M, &sum2};

  pthread_create(&thread1, NULL, thread_worker, (void*) &thread1_args);
  pthread_create(&thread2, NULL, thread_worker, (void*) &thread2_args);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("There are %d primes from 2 to %d\n", sum1 + sum2, M);

  return 0;
}
