#include<stdio.h>
#include<unistd.h>
#include<malloc.h>
#include<fcntl.h>

int main() {
    char *buf;
    buf = malloc(200*sizeof(char));

    for (int i = 0 ; i < 199 ; i++) {
        buf[i] = 'a' + (i%26);
    }

    int f = open("./test.txt",O_WRONLY);
    write(f,buf,150);

    close(f);
}