#include <stdio.h>
#include <stdlib.h>

#define FAST 1

int main(int argc, char **argv)
{
   int i;
   int a = 1, b = 2, c = 3, d = 1;
   float x = 0.1, y = 0.5, z = 0.33;
   printf("%d %d %d %d, %f %f %f\n", a, b, c, d, x, y, z);

#if FAST

   const float mul = 1 / 1.33;

   for (i=0; i<50000000; i++)
   {
      c = d << 1;                      // d * 2^1
      b = c*15;                        // ???
      a = b >> 4;                      // b / 2^4
      d = (unsigned) b / (unsigned) a; // unsigned

      y = 0.66;    // constants
      x = y * mul; // fp *
      z = x * mul; // fp *
   }

#else

   for (i=0; i<50000000; i++)
   {
      c = d*2;
      b = c*15;
      a = b/16;
      d = b/a;

      z = 0.33;
      y = 2*z;
      x = y / 1.33;
      z = x / 1.33;
   }

#endif

   printf("%d %d %d %d, %f %f %f\n", a, b, c, d, x, y, z);
   return 0;
}
