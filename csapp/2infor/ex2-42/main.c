#include <stdio.h>
#include <stdlib.h>

int32_t div16(int32_t x) {
    // 除以16，那么若x > 0,加上偏置0，若x < 0，加上偏置(1<<4)-1 = 15,然后移位
    // 题目要求不能用条件语句
    // x>>31得到符号位扩展，为了得到15或者0，只保留低4位
    int32_t bias = (x >> 31) & 0xF;
    return (x + bias) >> 4;
}

int main() {
    int32_t y1 = -47, y2 = -49, y3 = 47, y4 = 49;
    int32_t z1 = div16(y1), z2 = div16(y2), z3 = div16(y3), z4 = div16(y4);
    printf("%d %d %d %d\n", z1, z2, z3, z4);
}