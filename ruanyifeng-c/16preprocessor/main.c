#pragma c20
#include <stdio.h>
#define ID(n) i##n 
#define STR(x) #x
#define SSTR(x) STR(x) 

int main() {
    int ID(1), ID(2);
    printf("%d %d\n", i1, ID(2));

    // 替换成 #__LINE__ ，字符串化在参数替换之前就开始了
    printf("%s\n", STR(__LINE__));
    // 替换成 STR(13)，再替换成 #13
    printf("%s\n", SSTR(__LINE__));

    printf("This function: %s\n", __func__);
    printf("This file: %s\n", __FILE__);
    printf("Compiled on: %s %s\n", __DATE__, __TIME__);
    printf("C Version: %ld\n", __STDC_VERSION__);
}