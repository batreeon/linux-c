#include <arpa/inet.h>
#include <stdio.h>

int main(int argc,char **argv) {
    if (argc < 2) {
        printf("want 2 arguments\n");
        return;
    }
    struct sockaddr_in addr;
    sscanf(argv[1],"%x",&addr.sin_addr.s_addr);
    addr.sin_addr.s_addr = htonl(addr.sin_addr.s_addr);
    char dst[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&addr.sin_addr,dst,INET_ADDRSTRLEN);
    puts(dst);
}