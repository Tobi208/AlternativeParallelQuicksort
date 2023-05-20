/**********************************************************************
 * Enumeration sort
 *
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS	5
#define len 100000

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

double indata[len];
double outdata[len];

void* findrank(void* arg) {
  long j, t = (long) arg;
	int rank, i;

  for (j = t; j < len; j += NUM_THREADS) {
    rank = 0;
    for (i = 0; i < len; i++)
      if (indata[i] < indata[j])
        rank++;
	  outdata[rank] = indata[j];
  }

  return NULL;
}

int main(int argc, char *argv[]) {
	
  pthread_t threads[NUM_THREADS];
  int i;
  long t;
  
  // Generate random numbers
  for (i = 0; i < len; i++){
      indata[i] = drand48();
      outdata[i] = -1.0;
  }

  // Enumeration sort
  double startTime = get_wall_seconds();
  for(t = 0; t < NUM_THREADS; t++)
    pthread_create(&threads[t], NULL, findrank, (void*) t);
  
  for(t=0; t < NUM_THREADS; t++) 
    pthread_join(threads[t], NULL);

  double timeTaken = get_wall_seconds() - startTime;
  printf("Time: %f  NUM_THREADS: %d\n", timeTaken, NUM_THREADS);

  // Check results, -1 implies data same as the previous element
  for (i = 0; i < len - 1; i++)
    if (outdata[i] > outdata[i+1] && outdata[i+1] > -1)
      printf("ERROR: %f,%f\n", outdata[i], outdata[i+1]);

  return 0;
}
