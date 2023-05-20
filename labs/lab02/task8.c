#include <stdio.h>

int main() {

    double x = 1.0;
    int y = 2;
    char z = 'c';

    printf("DOUBLE: value %f / pointer %p / size %lu\n", x, &x, sizeof(x));
    printf("INT:    value %d / pointer %p / size %lu\n", y, &y, sizeof(y));
    printf("CHAR:   value %c / pointer %p / size %lu\n", z, &z, sizeof(z));

}