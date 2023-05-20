#include "testfuncs.h"

void f_std(const double * __restrict a, 
	   const double * __restrict b, 
	   double * __restrict c, int N) {
  int i;
  double x = 0.3;
  for(i = 0; i < N; i++) {
    c[i] = a[i]*a[i] + b[i] + x;
  }
}

void f_opt(const double * __restrict a, 
	   const double * __restrict b, 
	   double * __restrict c, int N) {
  
  
  int i;
  double x = 0.3;

  // unroll loop
  for(i = 0; i <= N - 4; i += 4) {
    c[i  ] = a[i  ]*a[i  ] + b[i  ] + x;
    c[i+1] = a[i+1]*a[i+1] + b[i+1] + x;
    c[i+2] = a[i+2]*a[i+2] + b[i+2] + x;
    c[i+3] = a[i+3]*a[i+3] + b[i+3] + x;
  }

  // cover un-unrollable indices
  for (i = N - N % 4; i < N;  i++)
    c[i] = a[i]*a[i] + b[i] + x;
}

