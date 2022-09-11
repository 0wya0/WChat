#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

#define C_ERROR -1
#define C_FAILURE -1
#define C_SUCCESS 0

//连接服务器
int client_connect(const char *ip, int port);
//向服务器发送消息
int client_send(const void *buf, int buf_len);
//从客户端接收消息
int client_recv(void *buf, int buf_len);
//获取服务器的socket
int get_sockfd();
//关闭socket
int client_close();

#endif 