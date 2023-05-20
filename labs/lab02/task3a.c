#include <stdio.h>

int main() {

    int a, b;
    printf("Input: ");
    scanf("%d %d", &a, &b);

    if (a % 2 == 0 && b % 2 == 0)
        printf("sum: %d\n", a + b);
    else
        printf("prod: %d\n", a * b);

}
