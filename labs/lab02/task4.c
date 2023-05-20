#include <stdio.h>

int main() {

    float a, b;
    char op;
    printf("Input: ");
    scanf("%f %c %f", &a, &op, &b);

    switch(op) {
        case('+'):
            printf("Output: %f\n", a + b);
            break;
        case('*'):
            printf("Output: %f\n", a * b);
            break;
        case('-'):
            printf("Output: %f\n", a - b);
            break;
        case('/'):
            printf("Output: %f\n", a / b);
            break;
        default:
            printf("Invalid operator: %c\n", op);
    }

}