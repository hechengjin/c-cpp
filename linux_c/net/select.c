#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int IoSelectTest();
int OOBSelectTest(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    OOBSelectTest(argc, argv);
//    IoSelectTest();
    return 0;
}


int OOBSelectTest(int argc, char* argv[])
{
    if(argc <= 2)
    {
        printf("usage: ip address + port numbers\n");
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    printf("ip: %s\n", ip);
    printf("port: %d\n", port);
    
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0 )
    {
        printf("Fail to create listen socket!\n");
        return -1;
    }

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
        printf("Fail to bind socket!\n");
        return -1;
    }
    ret = listen(listenfd, 5); //监听队列最大排除数设置为5
    if(ret == -1)
    {
        printf("Fail to listenfd socket!\n");
        return -1;
    }

    struct sockaddr_in client_address; //记录进行连接的客户端的地址
    socklen_t client_addrlength = sizeof(client_address);
    //只能接收一个客户端请求
    int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
    if(connfd < 0)
    {
        printf("Fail to accept!\n");
        close(listenfd);
    }
    
    printf("new client fd: %d\n", connfd);

    char buff[1024]; //数据接收缓冲区
    fd_set read_fds; //读文件操作符
    fd_set exception_fds; //异常文件操作符
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while(1)
    {
        memset(buff, 0, sizeof(buff));
        //每次调用select之前都要重新在read_fds和exception_fds中设置文件描述符connfd,因为事件发生后，文件描述符集合将被内核修改
        FD_SET(connfd, &read_fds);
        FD_SET(connfd, &exception_fds);

        ret = select(connfd+1, & read_fds, NULL, &exception_fds, NULL);
        if(ret < 0)
        {
            printf("Fail to select!\n");
            return -1;
        }

        if(FD_ISSET(connfd, &read_fds))
        {
            ret = recv(connfd, buff, sizeof(buff)-1,0);
            if(ret <= 0)
            {
                break;
            }
            printf("get %d bytes of normal data: %s \n", ret, buff);
        }
        else if(FD_ISSET(connfd, &exception_fds))
        {
            ret = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
            if(ret <= 0)
            {
                break;
            }
            printf("get %d bytes of exception data: %s \n", ret, buff);
        }
    }

    close(connfd);
    close(listenfd);

    return 0;
}

int IoSelectTest()
{

    fd_set rd;
    struct timeval tv;
    int err;

    FD_ZERO(&rd);
    //0是标准输入的文件描述符，运行程序后，输入任何字符然后回车即会打印 data is available!
    FD_SET(0, &rd);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = select(1, &rd, NULL, NULL, &tv);

    if( err == 0) //超时
    {
        printf("select time out ! \n");
    }
    else if(err == -1) //失败
    {
        printf("fail to select!\n");
    }
    else //成功
    {
        printf("data is available!\n");
    }
    return 0;
}
