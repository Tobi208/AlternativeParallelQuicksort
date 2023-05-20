#include <stdio.h>
#include <pthread.h>

void* the_thread_func(void* arg) {
  /* Do something here? */
  
  double* data = (double*) arg;
  printf("data in the_thread_func() = %.1f %.1f %.1f\n", data[0], data[1], data[2]);
  
  return NULL;
}

int main() {
  printf("This is the main() function starting.\n");

  double data_for_thread[3] = { 5.7, 9.2, 1.6 };
  double data_for_threadB[3] = { 7.5, 2.9, 6.1 };

  /* Start thread. */
  pthread_t thread;
  pthread_t threadB;
  printf("the main() function now calling pthread_create().\n");
  pthread_create(&thread, NULL, the_thread_func, data_for_thread);
  pthread_create(&threadB, NULL, the_thread_func, data_for_threadB);

  printf("This is the main() function after pthread_create()\n");

  /* Do something here? */

  /* Wait for thread to finish. */
  printf("the main() function now calling pthread_join().\n");
  pthread_join(thread, NULL);
  pthread_join(threadB, NULL);

  return 0;
}
