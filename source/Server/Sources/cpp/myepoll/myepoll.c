#include "myepoll.h"

int epoll_fd;

//创建epoll对象
void myepoll_create(int size)
{
    //创建epoll对象
    epoll_fd = epoll_create(size);
    if(-1 == epoll_fd)
    {
        perror("epoll_create error");
    }
}

//将fd添加到epoll监听的集合中
void myepoll_add_in(int fd)
{
    struct epoll_event ev;
    //设置监听socket可读
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    //将所有需要监听的socket的添加到epoll_fd
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD,fd, &ev))
    {
        perror("epoll_add error");
    }
}

//阻塞等待事件发生
int myepoll_wait_block(struct epoll_event *events, int size)
{
    //阻塞监听
    int num = epoll_wait(epoll_fd, events, size, -1);
    if(-1 == num)
    {
        perror("myepoll_wait_block error");
        return -1;
    }
    return num;
}

//删除并关闭
void myepoll_delete_in(int fd)
{
    //从epoll监听删除下线fd
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(0 != epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev))
    {
        perror("myepoll_delete epoll_ctl error");
        return ;
    }
}

//销毁
void mypoll_destroy()
{
    //关闭epoll对象的文件描述符
    if(-1 == close(epoll_fd))
    {
        perror("myepoll_destroy close error");
    }
}
