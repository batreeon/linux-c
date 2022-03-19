#include <stdio.h>

int main() {
    // 怎么恢复呢？？
    freopen("stdout.txt", "w", stdout);
    printf("freopen");
}