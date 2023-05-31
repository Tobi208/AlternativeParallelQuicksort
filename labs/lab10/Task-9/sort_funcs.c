#include <stdlib.h>
#include <omp.h>
#include "sort_funcs.h"

void merge_sort(intType* list_to_sort, int N, int T) {
  if(N == 1) {
    // Only one element, no sorting needed. Just return directly in this case.
    return;
  }
  int n1 = N / 2;
  int n2 = N - n1;

  //Allocate new lists
  intType* buff = (intType*)malloc(N * sizeof(intType));
  intType* list1 = buff;
  intType* list2 = buff + n1;
  // intType* list1 = (intType*)malloc(n1 * sizeof(intType));
  // intType* list2 = (intType*)malloc(n2 * sizeof(intType));

  int i;
  for(i = 0; i < n1; i++)
    list1[i] = list_to_sort[i];
  for(i = 0; i < n2; i++)
    list2[i] = list_to_sort[n1+i];

  // Sort list1 and list2
  if (T > 1) {
    #pragma omp parallel num_threads(2)
    {
      if (omp_get_thread_num() == 0)
        merge_sort(list1, n1, T/2);
      else
        merge_sort(list2, n2, T/2);
    }
  } else {
    merge_sort(list1, n1, 0);
    merge_sort(list2, n2, 0);
  }
  
  // Merge!
  i = 0;
  int i1 = 0, i2 = 0;
  while(i1 < n1 && i2 < n2)
    if(list1[i1] < list2[i2])
      list_to_sort[i++] = list1[i1++];
    else
      list_to_sort[i++] = list2[i2++];
  while(i1 < n1)
    list_to_sort[i++] = list1[i1++];
  while(i2 < n2)
    list_to_sort[i++] = list2[i2++];

  free(buff);
  // free(list1);
  // free(list2);
}

