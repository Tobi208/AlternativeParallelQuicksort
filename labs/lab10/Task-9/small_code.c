#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char* argv[]) {

    // enable nested parallelism
    omp_set_nested(0);

#pragma omp parallel num_threads(2)
{
    printf("t%d - outer\n", omp_get_thread_num());

    #pragma omp parallel num_threads(2)
    {
        printf("t%d - inner\n", omp_get_thread_num());
    }
}

    return 0;
}
