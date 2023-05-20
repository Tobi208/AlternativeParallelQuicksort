#include <stdio.h>

int main() {

    FILE *file = fopen("data.txt", "r");

    int n;
    fscanf(file, "%d", &n);

    double price;
    char product[50];
    for (int i = 0; i < n; i++) {
        fscanf(file, "%s %lf", product, &price);
        printf("%s %.1lf\n", product, price);
    }

    fclose(file);

}
