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

#define C_ERROR -1
#define C_FAILURE -1
#define C_SUCCESS 0

/**
 * @brief  
 * @note   请求连接服务器
 * @param  *ip:     服务器的网络地址
 * @param  port:    服务器使用的端口号
 * @retval 成功返回服务器的Socket的文件描述符; 失败客户端退出
 */
void client_connect(const char *ip, int port);

int client_send(const void *buf, int buf_len);

int client_recv(void *buf, int buf_len);

void client_close();

#endif 