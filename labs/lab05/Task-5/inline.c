#define FAST 1

#if FAST

static inline int square(int x) {
    x * x;
}

#else

static int square(int x) {
    x * x;
}

#endif



int main(int argc, char **argv)
{
    int i;
    float x;

    for (i=0; i<500000000; i++)
    {
        square(x);
    }

    return 0;
}
