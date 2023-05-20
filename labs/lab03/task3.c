#include <stdio.h>

void print_int_1(int x) {
    printf("Here is the number: %d\n", x);
}

void print_int_2(int x) {
    printf("Wow, %d is really an impressive number!\n", x);
}

int main(int argc, char** argv) {

    void (*foo) (int);

    foo = &print_int_1;
    foo(1);

    foo = &print_int_2;
    foo(2);

    return 0;
}
