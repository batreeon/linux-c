#include "csapp.h"

int main(int argc,char **argv) 
{
    // 分别用来指向addrinfo链表中的一个元素，指示链表，用作getaddrinfo的参数
    struct addrinfo *p,*listp,hints;
    int rc;

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

    // 不使用getnameinfo
    for (p = listp; p; p = p->ai_next) {
        char dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET,p->ai_addr->sa_data,dst,INET_ADDRSTRLEN);
        printf("%s\n",dst);
    }

    // free
    freeaddrinfo(listp);

    exit(0);
}