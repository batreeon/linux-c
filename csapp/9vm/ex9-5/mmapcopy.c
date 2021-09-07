#include<sys/mman.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<stdio.h>

void mmapcopy(int fd, int size) {
    // 将fd指向的文件映射到p所得虚拟地址
    char *p;
    p = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    write(1, p, size);
    printf("\n");
    return;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("需要指定文件名！");
        exit(0);
    }

    int fd;
    struct stat stat;

    fd = open(argv[1], O_RDONLY, 0);
    fstat(fd, &stat);
    mmapcopy(fd, stat.st_size);
    exit(0);
}