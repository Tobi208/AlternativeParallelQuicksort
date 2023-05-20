#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{

    double last_finite;
    
    printf("\n --- large float --- \n");
    float large_float = (float) pow(10, 25);
    while (!isinf(large_float)) {
        last_finite = large_float;
        large_float *= 10;
    }
    printf("%f\n\n", last_finite);

    printf(" --- small float --- \n");
    float small_float = -1 * pow(10, 25);
    while (!isinf(small_float)) {
        last_finite = small_float;
        small_float *= 10;
    }
    printf("%f\n\n", last_finite);

    printf(" --- large double --- \n");
    double large_double = pow(2, 290);
    while (!isinf(large_double)) {
        last_finite = large_double;
        large_double *= 10;
    }
    printf("%f\n\n", last_finite);

    printf(" --- small double --- \n");
    double small_double = -1 * pow(2, 290);
    while (!isinf(small_double)) {
        last_finite = small_double;
        small_double *= 10;
    }
    printf("%f\n\n", last_finite);


    printf(" --- not a number --- \n");
    float imaginary = sqrt(-25);
    printf("%lf\n\n", imaginary);

    printf(" --- continued use of NaN and INF --- \n");
    printf("Nan: %lf\n", imaginary * 2);
    printf("Inf: %lf\n", large_float * 2);


    return 0;
}
