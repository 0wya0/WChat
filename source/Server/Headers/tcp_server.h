#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TCP_SUCCESS 1
#define TCP_FAILURE -1
#define EXIT_ERROR -1 

#define LISTEN_NUM 10       //listen函数同时处理请求的最大数

//server_create 包括创建(socket)、绑定(bind)、监听listen
int server_create(const char *ip, int port);
//封装的accept函数，阻塞等待客户端连接
int server_accept(int sockfd);
//发送信息
int send_msg(int cfd, const void *buf, int buf_len);

#endif