#include "../../code/include/csapp.h"

int main(int argc, char **argv, char **envp) {
    printf("Command-int arguments:\n");
    for (int i = 0; argv; argv++) {
        printf("\targv[%2d]: %s\n", i, argv[i]);
        i++;
    }
    for (int i = 0; envp; envp++) {
        printf("\tenvp[%2d]: %s\n", i, envp[i]);
        i++;
    }
}