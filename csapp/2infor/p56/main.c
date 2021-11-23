#include <stdio.h>

int main() {
    short sx = -12345;
    unsigned uy = sx;
    printf("uy = %u\n", uy); //4294954951

    printf("%u\n", (unsigned)(int)sx); //4294954951
    printf("%u\n", (int)(unsigned short)sx); //53191
}