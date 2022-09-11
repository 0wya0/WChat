#include "myepoll.h"
#include "server_func.h"
#include "tcp_server.h"
#include "online.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define IP "127.0.0.1"
#define PORT 6666
#define EVENTS_MAX_SIZE 256

void signalhandler(int signum)
{
    if(SIGINT == signum)    //ctl+C退出程序
    {   
        //销毁在线用户链表
        ol_destroy();
        //关闭数据库,保存id_counter
        id_destroy();
        //销毁线程池
        exit(0);
    }
    else    //其他异常捕捉打印信号值
    {
        printf("signum = %d\n", signum);
    }
}
    
int main(int argc, char const *argv[])
{
    //屏蔽异常
    for(int i = 1; i < 65; i++)
    {
        signal(i, signalhandler);
    }
    
    //创建服务器套接字并绑定网络地址和端口、设置监听
    int sockfd = server_create(IP, PORT);
    //创建线程池
    tpool_create(10);
    //创建在线用户链表
    id_init();
    //创建在线用户链表
    if(OL_FAILURE == ol_create())
    {
        printf("ol_create error");
        exit(-1);
    }

    //初始化epoll对象
    myepoll_create(EVENTS_MAX_SIZE);
    struct epoll_event events[EVENTS_MAX_SIZE] = {0};
    //将服务器Socket的文件描述符sockfd添加到epoll监听的集合中
    myepoll_add_in(sockfd);
    int i;    
    char buf[2000] = {0}; 
    while(1)
    {
        //num用于保存发起请求的socket数
        int num = myepoll_wait_block(events, EVENTS_MAX_SIZE);
        for(i = 0; i < num; i++)
        {
            if(events[i].data.fd == sockfd)     //客户端发起连接服务器
            {
                //cfd用于保存发起请求客户端的socket文件描述符
                int cfd = server_accept(sockfd);
                if(-1 == cfd)
                {
                    continue;
                }
                //将客户端的cfd加入到epoll监听的集合中
                myepoll_add_in(cfd);
            }
            else    //客户端发消息
            {
                if(events[i].events & EPOLLIN)
                {
                    memset(buf, 0, sizeof(buf));
                    int ret = recv(events[i].data.fd, buf, sizeof(buf), 0);
                    if(-1 == ret)
                    {
                        if(errno == EINTR)
                        {
                            continue;
                        }
                        perror("main-recv");
                    }
                    else if(0 == ret)   //用户下线
                    {
                        printf("客户端(cfd = %d)已下线!\n", events[i].data.fd);
                        //从用户在线队列中删除
                        ol_delete_bycfd(events[i].data.fd);
                        //从epoll集合中删除, 并关闭该fd     
                        myepoll_delete_in(events[i].data.fd);
                        close(events[i].data.fd);
                    }
                    else    //将用户发送的消息添加到任务队列中
                    {
                        ctl_t *ctl = (ctl_t *)buf;
                        ctl->cfd = events[i].data.fd;
                        void *tmp = calloc(1, ctl->msg_size);
                        if(NULL == tmp)
                        {
                            perror("main-calloc");
                            continue ;
                        }
                        memcpy(tmp, buf, ctl->msg_size);
                        tpool_add_task(task_routine, tmp);
                    }
                }
            }
        }
    }
    return 0;
}