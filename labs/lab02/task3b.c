#include <stdio.h>
#include <math.h>

int main() {

    float a, b, c;
    printf("Input: ");
    scanf("%f %f %f", &a, &b, &c);

    if (fabs(a) >= fabs(b) && fabs(a) >= fabs(c))
        printf("%f\n", a);
    else if (fabs(b) >= fabs(c))
        printf("%f\n", b);
    else
        printf("%f\n", c);

}