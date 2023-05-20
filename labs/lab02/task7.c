#include <stdio.h>

int main() {

    int a, number, remainder, reversed;
    printf("Input: ");
    scanf("%d", &a);

    number = a;
    reversed = 0;

    while (number > 0) {
        remainder = number % 10;
        reversed = reversed * 10 + remainder;
        number /= 10;
    }

    if (reversed == a)
        printf("Output: this is a palindrome\n");
    else
        printf("Output: this is not a palindrome\n");

}
