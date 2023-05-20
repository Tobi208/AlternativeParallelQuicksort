#include <stdio.h>
#include <math.h>

int main() {

    float a, b, c;
    printf("Input: ");
    scanf("%f %f %f", &a, &b, &c);

    if ((fabs(a) >= fabs(b) && fabs(a) <= fabs(c))
     || (fabs(a) >= fabs(c) && fabs(a) <= fabs(b)))
        printf("%f\n", a);
    else if ((fabs(b) >= fabs(a) && fabs(b) <= fabs(c))
          || (fabs(b) >= fabs(c) && fabs(b) <= fabs(a)))
        printf("%f\n", b);
    else
        printf("%f\n", c);

}