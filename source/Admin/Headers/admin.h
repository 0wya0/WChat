 #ifndef __ADMIN_H__
#define __ADMIN_H__

#include "tcp_client.h"

#include <stdio.h>
#include <string.h>

#define SIZE 1024

//消息的传输协议
enum command
{
    OPT_ERR = -1,
    CHECK_ONLINE = 11,            //查看在线用户
    REMOVE_USER,                 //踢人
    CHECK_ALL,                  //查看所有用户
    BAN,                       //禁言    
    NOT_BAN,                  //解除禁言
};

//消息的控制信息
typedef struct control
{
    int cmd;                //消息要执行的操作
    int cfd;                //用于保存客户端的Socket
    int msg_size;           //消息的大小
}ctl_t;
//管理员信息结构
typedef struct
{
    ctl_t ctl;                  //控制信息
    char sql_buf[SIZE];     //要执行的SQL语句
}msg_sql_t;                    //和服务器msg_sql_t结构相同;

typedef struct
{
    ctl_t ctl;                  //控制信息
    char who_id[11];
    char sql_buf[SIZE];         //要执行的SQL语句
}msg_who_t;

typedef struct server_message   
{
    ctl_t ctl;                   //操作: -1代表操作失败(任意一个)
    char reply[SIZE];            //服务器给客户端的消息
}ser_msg_t;


void admin_login();

void home_func();

#endif