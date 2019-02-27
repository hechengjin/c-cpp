#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>
#include <poll.h>
#include <sys/stropts.h>
#include <signal.h>
#define MAXLINE 5
#define OPEN_MAX 1024

int main()
{
    int listenfd, connfd, sockfd, i, maxi, timeout;
    int nready;
    socklen_t clilen;
    ssize_t n;
    char buf[MAXLINE];
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in servaddr, cliaddr;
    //创建监听套接字
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() error!\n");
        exit(0);
    }
    //先对协议地址进行清零
    bzero(&servaddr, sizeof(servaddr));
    //设置为IPv4 或 IPv6
    servaddr.sin_family = AF_INET;
    //绑定端口
    servaddr.sin_port = htons(8888);
    //任何一个IP地址，让内核自行选择
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //加上下面这个要不printf打不出来
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

    //绑定套接口到本地协议地址
    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind() error!");
        exit(0);
    }
    printf("server start at port:8888 \n");
    //服务器开始监听
    if(listen(listenfd, 5) < 0)
    {
        printf("listen() error!");
        exit(0);
    }
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM; //关心监听套接字的读事件
    for(i = 1; i < OPEN_MAX; ++i)
    {
        client[i].fd = -1;
    }
    maxi = 0;
    for(;;)
    {
        timeout = 3000; //超时时间 单位 毫秒
        nready = poll(client, maxi + 1, timeout); //基于链表来存储，没有最大连接数限制,“水平触发”，即没有被处理，下次还会报告该fd
        if(nready < 0)
        {
            printf("poll fail and errno %d\n", errno);
        }
        else if(0 == nready)
        {
            printf("timeout and start the next. \n");
            continue;
        }
        if(client[0].revents & POLLRDNORM)
        {
            clilen = sizeof(cliaddr);
            //accept后面两个参数是保留远程电脑的信息，如果不管远程电脑的信息，可以设置为NULL
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            if(-1 == connfd)
                printf("accept fail\n");
            printf("accept a new client : %s:%d fd:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port,connfd);
            if(connfd < 0)
            {
                continue;
            }
            for(i = 1; i < OPEN_MAX; ++i)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }
            if(i == OPEN_MAX)
            {
                printf("too many clients");
                exit(0);
            }
            client[i].events = POLLRDNORM;
            if(i > maxi)
            {
                maxi = i;
            }
           if(--nready <= 0)
              continue;
        }
        for(i = 1; i < OPEN_MAX; ++i)
        {
            if(5 == i)
            {
                printf("client:%d\n",client[i].fd);
            }
            sockfd = client[i].fd;
            if(sockfd < 0)
            {
                continue;
            }
            printf("read from fd: %d\n", sockfd);
            if(client[i].revents & (POLLRDNORM | POLLERR))
            {
                if((n = read(sockfd, buf, MAXLINE)) < 0 )
                {
                    if(errno == ECONNRESET)
                    {
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                    {
                        printf("read error!\n");
                    }
                }
                else if(n == 0)
                {
                    printf("close fd:%d\n",sockfd);
                    close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    printf("write to fd:%d\n", sockfd);
                    write(sockfd, buf, n);
                }
                if(--nready <= 0)
                    break;
            }
        }
    }
}

