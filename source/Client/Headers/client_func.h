#ifndef __FUNC_H__
#define __FUNC_H__

#include "tcp_client.h"
#include "myfile.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

//用户消息的传输协议
enum command
{
    OPT_ERR = -1,           //操作有误，主要用于服务器回复客户端
    
    REGISTER = 0,           //注册
    LOGIN,                  //登录
    FIND_PASSWORD,          //找回密码
    PRIVATE_CHAT,           //私聊
    GROUP_CAHT,             //群聊
    CHECK_FILE,             //查看群文件
    UPLOAD_FILE,            //上传群文件
    DOWNLOAD_FILE,          //下载群文件
    UPDATE,                 //更新
    LEAVE_WORD,             //留言
    LOOK_WORD,              //查看留言
    CHECK_ONLINE,           //查看在线用户
    REMOVE_USER,            //踢人
};

//消息的控制信息
typedef struct control
{
    int cmd;                //消息要执行的操作
    int cfd;                //用于保存客户端的Socket
    int msg_size;           //消息的大小
}ctl_t;
//生日
typedef struct birthday
{
    int year;
    int month;
    int day;
}birthday_t;
//保存个人信息
typedef struct information
{
    int age;                        //年龄
    char sex[4];                    //性别: 男 女
    char myid[11];                  //账号ID  固定10字节
    char tel[12];                   //电话号码
    birthday_t bday;                //生日
    char password[21];              //密码--最大21字节
    char security[21];              //密保--最大21字节
    char nickname[33];              //昵称--最大32字节
}info_t;

/**************************客户端消息结构定义**************************/
#define SIZE 1024

//与服务器数据库交互类信息：注册、找回密码，修改
typedef struct
{
    ctl_t ctl;                  //控制信息
    char sql_buf[SIZE];         //要执行的SQL语句
}msg_sql_t;

//登录
typedef struct
{
    ctl_t ctl;                  //控制信息
    char login_id[11];
    char sql_buf[SIZE];         //要执行的SQL语句
}msg_login_t;

//与其他客户端聊天类信息, 需要转发：私聊、群发，发文件
typedef struct
{
    ctl_t ctl;
    char from_id[11];
    char to_id[11];             //接收消息的用户的ID, 群发置空
    char from_name[33];         //发送者的昵称
    char chat_buf[SIZE];        //消息内容
}msg_chat_t;

/****************************文件消息**************************************/
typedef struct
{
    ctl_t ctl;
    int flags;
    long file_size;
    char filename[128];
    char from_id[11];
    char file_buf[SIZE];        //消息内容
}msg_file_t;

/****************************服务器消息结构定义*****************************/
typedef struct server_message   
{
    ctl_t ctl;                   //操作: -1代表操作失败(任意一个)
    char reply[1024];            //服务器给客户端的消息
}ser_msg_t;



//首页程序
void face(const char *ip, int port);
//主页 
void home();
//子进程消息接收处理函数
void recv_msg_handler();


#endif