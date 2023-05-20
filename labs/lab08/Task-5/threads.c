#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* thread_worker(void* arg) {
  int i = * (int*) arg;
  printf("hello from thread %d\n", i);
  return NULL;
}

int main(int argc, char** argv) {

  const int N = atoi(argv[1]);

  /* Start thread. */
  pthread_t* threads = (void*) malloc(N * sizeof(pthread_t));
  int* indices = (void*) malloc(N * sizeof(int));
  for (int i = 0; i < N; i++) {
    indices[i] = i;
    pthread_create(&threads[i], NULL, thread_worker, &indices[i]);
  }

  for (int i = 0; i < N; i++)
    pthread_join(threads[i], NULL);

  free(threads);
  free(indices);
  return 0;
}
