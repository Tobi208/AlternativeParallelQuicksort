/**********************************************************************
 * This program calculates pi using C
 *
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 4

const int intervals = 500000000;
const double dx = 1.0 / intervals;

double local_sums[N];

void* thread_worker(void* index) {
  double x, local_sum = 0;
  for (long i = ((long) index) + 1; i <= intervals; i += N) {
    x = dx * (i - 0.5);
    local_sum += dx * 4.0 / (1.0 + x * x);
  }
  local_sums[(long) index] = local_sum;
  return NULL;
}

int main(int argc, char *argv[]) {

  long i;
  double sum = 0.0;

  pthread_t threads[N];
  for (i = 0; i < N; i++)
    pthread_create(&threads[i], NULL, thread_worker, (void*) i);

  for (i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
    sum += local_sums[i];
  }

  printf("PI is approx. %.16f\n",  sum);

  return 0;
}
