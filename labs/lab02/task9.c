#include <stdio.h>
#include <string.h>

void swap_nums(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swap_pointers(char** p1, char** p2) {
    char* temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

int main() {

    int a = 3;
    int b = 4;
    char* s1 = "second";
    char* s2 = "first";

    swap_nums(&a, &b);
    printf("a=%d, b=%d\n", a, b);

    swap_pointers(&s1, &s2);
    printf("s1=%s, s2=%s\n", s1, s2);

    return 0;
}
