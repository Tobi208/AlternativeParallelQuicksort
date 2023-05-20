#include <stdio.h>
#include <string.h>

typedef struct product {
    char name[50];
    double price;
} product_t;

int main(int argc, char const *argv[]) {

    FILE *file = fopen(argv[1], "r");

    int n;
    fscanf(file, "%d", &n);

    product_t arr_of_prod[100];
    double price;
    char product[50];
    for (int i = 0; i < n; i++) {
        fscanf(file, "%s %lf", product, &price);
        strcpy(arr_of_prod[i].name, product);
        arr_of_prod[i].price = price;
    }

    fclose(file);

    for (int i = 0; i < n; i++)
        printf("%s %.1lf\n", arr_of_prod[i].name, arr_of_prod[i].price);

}
