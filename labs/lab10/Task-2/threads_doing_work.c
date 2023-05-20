#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void do_some_work() {
  printf("Now I am going to do some work...\n");
  long int i;
  long int j = 0;
  for(i = 0; i < 3000000000; i++) {
    j += 3;
  }
  printf("Work done! My result j = %ld\n", j);
}

int main(int argc, char** argv) {

  const int n = atoi(argv[1]);

  double timer = omp_get_wtime();

#pragma omp parallel num_threads(n)
  {
    do_some_work();
  }

  printf("n = %d, t = %.2f\n", n, omp_get_wtime() - timer);

  return 0;
}
