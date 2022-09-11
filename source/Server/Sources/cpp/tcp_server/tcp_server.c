#include "tcp_server.h"

int server_create(const char *ip, int port)
{
    int ret;
    //创建套接字：tcp,字节流，无附加协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        perror("server_create socket error");
        exit(EXIT_ERROR);
    }

    //设置地址可以重复绑定
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    //绑定信息，将sockfd与固定的ip和port绑定
    //用于保存服务器ip和port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;               //IPV4
    server_addr.sin_port = htons(port);             //转成网络的大端序，如果主机也为大端序则不需要转
    server_addr.sin_addr.s_addr = inet_addr(ip);    //字符型网络地址转32位二进制数据
    //绑定信息
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(-1 == ret)
    {
        perror("server_create bind error");
        exit(EXIT_ERROR);
    }

    //设置监听
    ret = listen(sockfd, LISTEN_NUM);       //允许同时LISTEN_NUM个连接请求
    if(-1 == ret)
    {
        perror("server_create listen error");
        exit(EXIT_ERROR);
    }

    return sockfd;
}

int server_accept(int sockfd)
{
    //用于保存请求连接的客户端的套接字信息: ip,port
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    memset(&client_addr, 0, addrlen);
    
    //阻塞等待客户端连接
    int cfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
    if(-1 == cfd)
    {
        perror("server_accept error");
        exit(EXIT_ERROR);
    }

    //输出客户端信息
    printf("客户端(ip: %s | port: %d): cfd = %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), cfd);

    return cfd;
}

int send_msg(int cfd, const void *buf, int buf_len)
{
    int ret = send(cfd, buf, buf_len, 0);
    if(-1 == ret)
    {
        perror("send_msg error");
        return TCP_FAILURE;
    }
    return TCP_SUCCESS;
}
