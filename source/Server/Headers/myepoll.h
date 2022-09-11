#ifndef __MYEPOOL_H__
#define __MYEPOOL_H__

#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//创建epoll
void myepoll_create(int size);
//将新的套接字fd添加到epoll集合中
void myepoll_add_in(int fd);
//阻塞等待事件的发生
int myepoll_wait_block(struct epoll_event *events, int size);
//将下线的客户端套接字fd从epoll集合中删除
void myepoll_delete_in(int fd);
//销毁epoll集合
void mypoll_destroy();

#endif
