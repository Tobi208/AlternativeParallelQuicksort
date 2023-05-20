#include <stdio.h>

int main() {

    int a, b;
    printf("Entered dividend: ");
    scanf("%d", &a);
    printf("Entered divisor: ");
    scanf("%d", &b);

    printf("Quotient: %d\n", a / b);
    printf("Remainder: %d\n", a % b);

}