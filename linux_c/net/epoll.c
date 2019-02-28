#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

int main(void)
{
    int const size = 5; //等同 const int size = 5;
    //监控文件描述符，内部使用红黑树,size无意义
    int epfd;
    //就绪的文件描述符个数，失败时返回-1 并设置errno
    int nfds;
    //ev用于epoll事件注册
    struct epoll_event ev;
   //events数组返回要处理的事件
    struct epoll_event events[size];//赋值的变量定义数据 c99支持 gcc/g++一般支持纯c编译器不支持
    epfd = epoll_create(1); //创建epoll内部存储结构, 1为监听的描述符数量，原来是用hash表存储，现在改为红黑树，size意义不大
    //针对每个fd设置相关文件描述符及状态和模式设置
    ev.data.fd = STDIN_FILENO;
    ev.events = EPOLLIN | EPOLLET; //监听读状态，同时设置ET模式
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);  //注册epoll事件
    for(;;)
    {
        nfds = epoll_wait(epfd, events, size, -1);
        for(int i = 0; i < nfds; i++)
        {
            if(events[i].data.fd == STDIN_FILENO)
                printf("Welcom to epoll's word!\n");
        }
    }
    

    return 0;

}
