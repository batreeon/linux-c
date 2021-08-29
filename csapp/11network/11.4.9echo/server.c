#include "csapp.h"

void echo(int connfd);

int main(int argc,char **argv)
{   
    // 监听描述符，已连接描述符
    int listenfd, connfd;
    socklen_t clientlen;
    // 空间足够大，可装下任何类型的套接字地址结构，保持协议无关性
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    // 监听描述符
    listenfd = Open_listenfd(argv[1]);
    while(1) 
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        echo(connfd);
        Close(connfd);
    }
    exit(0);
}