#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int CmpString(const void* a, const void* b) {
    return strcmp(*((char**) a), *((char**) b));
}

int main(int argc, char** argv) {

    char *arrStr[] = {"daa", "cbab", "bbbb", "bababa", "ccccc", "aaaa"};
    int arrStrLen = sizeof(arrStr) / sizeof(char *);
    qsort(arrStr, arrStrLen, sizeof(char *), CmpString);

    for (int i = 0; i < arrStrLen; i++) {
        printf("%s ", arrStr[i]);
    }
    printf("\n");

    return 0;
}
