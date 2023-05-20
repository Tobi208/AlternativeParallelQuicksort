#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* the_thread_func(void* arg) {
  /* Do something here? */
  int* xs = (void*) malloc(sizeof(int));
  xs[0] = 42;
  pthread_exit((void*) xs);
}

int main() {
  printf("This is the main() function starting.\n");

  /* Start thread. */
  pthread_t thread;
  printf("the main() function now calling pthread_create().\n");
  if(pthread_create(&thread, NULL, the_thread_func, NULL) != 0) {
    printf("ERROR: pthread_create failed.\n");
    return -1;
  }

  printf("This is the main() function after pthread_create()\n");

  /* Do something here? */

  /* Wait for thread to finish. */
  printf("the main() function now calling pthread_join().\n");
  int* xs;
  if(pthread_join(thread, (void**) &xs) != 0) {
    printf("ERROR: pthread_join failed.\n");
    return -1;
  }

  printf("Received value %d from worker\n", xs[0]);
  free(xs);

  return 0;
}
