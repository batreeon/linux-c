#include <arpa/inet.h>
#include <stdio.h>

int main(int argc,char **argv) {
    if (argc < 2) {
        printf("want 2 arguments\n");
        return;
    }
    struct sockaddr_in addr;
    inet_pton(AF_INET,argv[1],&addr.sin_addr);
    printf("0x%x\n",ntohl(addr.sin_addr.s_addr));
}