#include <stdio.h>
#include <stdlib.h>

int main() {
    char *val = getenv("HOME");

    if (val == NULL) {
        printf("connot find the HOME environment variable\n");
        return 1;
    }

    printf("Value: %s\n", val);
    return 0;
}