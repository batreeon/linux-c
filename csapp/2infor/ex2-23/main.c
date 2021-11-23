#include <stdio.h>

int f1(unsigned w) {
    return (int) ((w << 24) >> 24);
}

int f2(unsigned w) {
    return ((int) w << 24) >> 24;
}
int main() {
    printf("%d %d", f1(0x00000076), f2(0x000000c9));
}