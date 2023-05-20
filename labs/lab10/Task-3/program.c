#include <stdio.h>
#include <omp.h>

void thread_func() {
  int num_threads = omp_get_num_threads();
  int thread_num = omp_get_thread_num();
  printf("Num threads = %d, thread num = %d\n", num_threads, thread_num);
}

int main(int argc, char** argv) {

#pragma omp parallel num_threads(4)
  {
    thread_func();
  }

  return 0;
}
