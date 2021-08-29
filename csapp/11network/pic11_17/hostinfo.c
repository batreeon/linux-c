#include "csapp.h"

int main(int argc,char **argv) 
{
    // 分别用来指向addrinfo链表中的一个元素，指示链表，用作getaddrinfo的参数
    struct addrinfo *p,*listp,hints;
    char buf[MAXLINE];
    int rc,flags;

    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    // get lists of addrinfo records
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_STREAM; //用于连接
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)
    {
        // 出错了
        fprintf(stderr, "getaddrinfo error: %s\n",gai_strerror(rc));
        exit(1);
    }

    // 遍历链表，展示ip地址
    flags = NI_NUMERICHOST; //返回数字地址字符串，而不是返回域名
    for (p = listp; p; p = p->ai_next)
    {
        getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        printf("%s\n", buf);
    }

    // free
    freeaddrinfo(listp);

    exit(0);
}