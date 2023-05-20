#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* thread_worker_nested(void* arg) {
  printf("nested worker\n");
  return NULL;
}

void* thread_worker(void* arg) {
  pthread_t t1;
  pthread_t t2;

  printf("worker creating 2 threads\n");

  pthread_create(&t1, NULL, thread_worker_nested, NULL);
  pthread_create(&t2, NULL, thread_worker_nested, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  return NULL;
}

int main(int argc, char** argv) {

  pthread_t t1;
  pthread_t t2;

  printf("main creating 2 threads\n");

  pthread_create(&t1, NULL, thread_worker, NULL);
  pthread_create(&t2, NULL, thread_worker, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}
