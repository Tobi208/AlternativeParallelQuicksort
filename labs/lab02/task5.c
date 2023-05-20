#include <stdio.h>
#include <math.h>

int main() {

    int x;
    printf("Input: ");
    scanf("%d", &x);

    if(floor(sqrt(x)) == sqrt(x))
        printf("Output: Yes\n");
    else
        printf("Output: No\n");

}