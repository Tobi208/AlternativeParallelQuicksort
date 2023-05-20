/**********************************************************************
 * This program calculates pi using C
 *
 **********************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if(argc != 2) {
    printf("Please give 1 arg: number of threads to use.\n");
    return -1;
  }
  const int N = atoi(argv[1]);

  int i;
  const int intervals = 500000000;
  double sum, dx, x;

  dx  = 1.0/intervals;
  sum = 0.0;

#pragma omp parallel for reduction(+:sum) private(i, x) firstprivate(dx, intervals) num_threads(N)
  for (i = 1; i <= intervals; i++) { 
    x = dx*(i - 0.5);
    sum += dx*4.0/(1.0 + x*x);
  }

  printf("PI is approx. %.16f\n",  sum);

  return 0;
}
