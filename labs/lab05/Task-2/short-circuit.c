#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define FAST 0

int isMostlyTrue()
{
   int r90 = ((float)rand()/RAND_MAX < 0.90)?1:0;
   return r90;
}

int isMostlyFalse()
{
   int r10 = ((float)rand()/RAND_MAX < 0.10)?1:0;
   return r10;
}

int isFiftyFifty()
{
   int r50 = ((float)rand()/RAND_MAX < 0.50)?1:0;
   return r50;
}

int main(int argc, char **argv)
{
   int i, foo;
   srand (time(NULL));
   for (i=0; i<2000000; i++)
   {
      foo = 0;

#if FAST

      if(isMostlyTrue() || isMostlyFalse())
         foo += 1;
      
      if(isMostlyFalse() && isMostlyTrue())
         foo += 2;

      if((isFiftyFifty() && isMostlyTrue()) || (isMostlyFalse() && isFiftyFifty()) )
         foo += 4;

#else 

      if(isMostlyFalse() || isMostlyTrue())
         foo += 1;
      
      if(isMostlyTrue() && isMostlyFalse())
         foo += 2;

      if( (isFiftyFifty()&&isMostlyFalse()) || (isFiftyFifty()&&isMostlyTrue()) )
         foo += 4;

#endif                
   
   }
   printf("foo = %d\n", foo);
   return 0;
}
