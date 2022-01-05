#include <stdio.h>

// 每次运行结果不一样，栈随机化
int main() {
    long local;
    printf("local at %p\n", &local);
    return 0;
}