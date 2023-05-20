#include <stdio.h>

int main() {

    int a, b;
    printf("Input: ");
    scanf("%d %d", &a, &b);
    printf("Output:\n");

    for (int i = 0; i < a; i++) {
        putchar('*');
        for (int j = 1; j < b - 1; j++) {
            if (i == 0 || i == a - 1)
                putchar('*');
            else
                putchar('.');
        }
        putchar('*');
        putchar('\n');
    }
}