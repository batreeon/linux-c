#include "csapp.h"

int main(int argc,char **argv)
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) 
    {
        fprintf(stderr, "usage: %s <host> <port>\n",argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, atoi(port));
    // 设置rio的fd
    Rio_readinitb(&rio, clientfd);

    // 从流中读取
    while (Fgets(buf, MAXLINE, stdin) != NULL)
    {
        // 将从标准输入读到的写入套接字
        Rio_writen(clientfd, buf, strlen(buf));
        // 读取返回
        Rio_readlineb(&rio, buf, MAXLINE);
        // 向流中写
        Fputs(buf,stdout);
    }
    Close(clientfd);
    exit(0);
}