#include "admin.h"
#include "tcp_client.h"

#include <signal.h>

#define IP "127.0.0.1"
#define PORT 6666

// void signalhandler(int signum)
// {
//     exit(0);
// }

int main(int argc, char const *argv[])
{
    //登录管理员账号
    admin_login();
    
//   signal(SIGINT, signalhandler);
    //连接服务器
    client_connect(IP, PORT);
    //进入主页
    home_func();
    return 0;
}
