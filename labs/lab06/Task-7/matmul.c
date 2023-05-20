#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
 
static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}
int rand_int(int N)
{
  int val = -1;
  while( val < 0 || val >= N )
    {
      val = (int)(N * (double)rand()/RAND_MAX);
    }
  return val;
}

void allocate_mem(int*** arr, int n)
{
  int i;
  *arr = (int**)malloc(n*sizeof(int*));
  for(i=0; i<n; i++)
    (*arr)[i] = (int*)malloc(n*sizeof(int));
}

void free_mem(int** arr, int n)
{
  int i;
  for(i=0; i<n; i++)
    free(arr[i]);
  free(arr);
}

void fill_zeroes(int*** arr, int n) {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      (*arr)[i][j] = 0;
}

/* ijk */
void mul_ijk(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (i=0; i<n; i++)  {
    for (j=0; j<n; j++) {
      int sum = 0;
      for (k=0; k<n; k++) 
	      sum += a[i][k] * b[k][j];
      c[i][j] = sum;
    }
  }
}

/* ikj */
void mul_ikj(int n, int **a, int **b, int **c)
{
  int i, j, k, x;
  for (i=0; i<n; i++)  {
    for (k=0; k<n; k++) {
      x = a[i][k];
      for (j=0; j<n; j++) 
        c[i][j] += x * b[k][j];
    }
  }
}

/* jik */
void mul_jik(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (j=0; j<n; j++) {
    for (i=0; i<n; i++) {
      int sum = 0;
      for (k=0; k<n; k++)
	      sum += a[i][k] * b[k][j];
      c[i][j] = sum;
    }
  }
}

/* jki */
void mul_jki(int n, int **a, int **b, int **c)
{
  int i, j, k, x;
  for (j=0; j<n; j++) {
    for (k=0; k<n; k++) {
      x = b[k][j];
      for (i=0; i<n; i++)
	      c[i][j] += a[i][k] * x;
    }
  }
}

/* kij */
void mul_kij(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (k=0; k<n; k++) {
    for (i=0; i<n; i++) {
      int x = a[i][k];
      for (j=0; j<n; j++)
      	c[i][j] += x * b[k][j];   
    }
  }
}

/* kji */
void mul_kji(int n, int **a, int **b, int **c)
{
  int i, j, k, x;
  for (k=0; k<n; k++) {
    for (j=0; j<n; j++) {
      x = b[k][j];
      for (i=0; i<n; i++)
      	c[i][j] += a[i][k] * x;   
    }
  }
}


/* blocking kij */
void mul_kij_block(int n, int **a, int **b, int **c)
{
  const int B = 100;
  double sa[B*B];
  double sb[B*B];
  double sc[B*B];
  if(n % B != 0) {
    printf("Error: n not divisible by block size.\n");
    return;
  }
  int nb = n / B;

  int i, j, k, ib, jb, kb, i0, j0, k0;
  int x;

  for (kb = 0; kb < nb; kb++) {
    k0 = kb * B;

    for (jb = 0; jb < nb; jb++) {
        j0 = jb * B;

      // copy to submatrix
      for (k = 0; k < nb; k++)
        for (j = 0; j < nb; j++)
          sb[j * nb + k] = b[k * nb][j * nb];

      for (ib = 0; ib < nb; ib++) {
        i0 = ib * B;

        // copy to submatrix
        for (i = 0; i < nb; i++)
          for (k = 0; k < nb; k++)
            sa[k * nb + i] = b[i * nb][k * nb];

        // clear result submatrix
        for (i = 0; i < B * B; i++)
            sc[i] = 0;


        // do matrix matix on the submatrix
          for (k = 0; k < B; k++) {
            for (i = 0; i < B; i++) {
              for (j = 0; j < B; j++) {

                // I give up.
                sc[(i0 + i) * n + j0 + j] += 0; // a[i][k] * b[k][j];   
              
              }
            }
          }

      }

    }

  }

}

// blocked version I found on the internet for i, j, k

#define N 1024 // matrix dimensions
#define BS 32   // block dimensions

void matmult_blocked(double *A, double *B, double *C) {
    int i, j, k, i1, j1, k1;
    for (i = 0; i < N; i += BS) {
        for (j = 0; j < N; j += BS) {
            for (k = 0; k < N; k += BS) {
                for (i1 = i; i1 < i + BS; i1++) {
                    for (j1 = j; j1 < j + BS; j1++) {
                        for (k1 = k; k1 < k + BS; k1++) {
                            C[i1*N + j1] += A[i1*N + k1] * B[k1*N + j1];
                        }
                    }
                }
            }
        }
    }
}


int main()
{
  int i, j, n;
  int **a;
  int **b;
  int **c;
  double time;
  int Nmax = 100; // random numbers in [0, N]

  printf("Enter the dimension of matrices n = ");
  if(scanf("%d", &n) != 1) {
    printf("Error in scanf.\n");
    return -1;
  }

  allocate_mem(&a, n);

  for ( i = 0 ; i < n ; i++ )
    for ( j = 0 ; j < n ; j++ )
      a[i][j] = rand_int(Nmax);

  allocate_mem(&b, n);
 
  for ( i = 0 ; i < n ; i++ )
    for ( j = 0 ; j < n ; j++ )
      b[i][j] = rand_int(Nmax);

  allocate_mem(&c, n);

  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_ijk(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version ijk, time = %f\n",time);
  
  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_ikj(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version ikj, time = %f\n",time);

  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_jik(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version jik, time = %f\n",time);

  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_jki(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version jki, time = %f\n",time);

  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_kij(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version kij, time = %f\n",time);

  fill_zeroes(&c, n);
  time=get_wall_seconds();
  mul_kji(n, a, b, c);
  time=get_wall_seconds()-time;
  printf("Version kji, time = %f\n",time);

  /*
    printf("Product of entered matrices:\n");
 
    for ( i = 0 ; i < n ; i++ )
    {
    for ( j = 0 ; j < n ; j++ )
    printf("%d\t", c[i][j]);
 
    printf("\n");
    }
  */

  free_mem(a, n);
  free_mem(b, n);
  free_mem(c, n);

  return 0;
}
