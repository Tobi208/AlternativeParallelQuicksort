#include <stdio.h>
#include <stdlib.h>

typedef struct {
  short int arr[8];
  long int x;
} A;

typedef struct {
  long int x;
  short int arr[8];
} B;

int main(){
  A* a = (A*)malloc(sizeof(A));
  B* b = (B*)malloc(sizeof(B));

  /*
    This bug writes a 'short' int -1 to the 'long' int memory space of x
    This means there are some leading 0s and the value stored in x becomes 65535.
    However, valgrind cannot detect this error.
    Maybe because it doesn't go beyond the memory allocated to the structure?
  */
  a->x = 7;
  for(int i = 0; i < 9; i++) // changed i < 8 to i < 9
    a->arr[i] = -1;
  printf("a->x = %d\n", a->x);

  /*
    
  */
  b->x = 7;
  for(int i = 0; i < 9; i++) // changed i < 8 to i < 9
    b->arr[i] = -1;
  printf("b->x = %d\n", b->x);

  free(a);
  free(b);
  
  return 0;
}
