#include "tcp_client.h"

static int sockfd;

//连接服务器
void client_connect(const char *ip, int port)
{
    //创建服务器的socket，用于客户端连接服务器
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        perror("client_connect: socket error");
        exit(C_ERROR);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr =inet_addr(ip);

    int ret = connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(server_addr));
    if(-1 == ret)
    {
        perror("client_connect: connect error");
        close(sockfd);
        exit(C_ERROR);
    }
}
//向服务器发送消息
int client_send(const void *buf, int buf_len)
{
    int ret = send(sockfd, buf, buf_len, 0);
    if(-1 == ret)
    {
        perror("client_send: send error");
        return C_FAILURE;
    }
    return C_SUCCESS;
}
//从客户端接收消息
int client_recv(void *buf, int buf_len)
{
    int ret = recv(sockfd, buf, buf_len, 0);
    if(-1 == ret)
    {
        perror("client_recv: recv error");
        return C_FAILURE;
    }
    else if(0 == ret)
    {
        close(sockfd);
        exit(0);
    }
    return C_SUCCESS; 
}
//关闭sockfd
void client_close()
{
    if(-1 == close(sockfd))
    {
        perror("client_close: recv error");
        exit(C_ERROR);
    }
}