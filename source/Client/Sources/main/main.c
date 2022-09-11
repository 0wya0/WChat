#include "tcp_client.h"
#include "client_func.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define IP "127.0.0.1"
#define PORT 6666


void signalhandler(int signum)
{  
    exit(0);
}

int main(int argc, char const *argv[])
{
//    signal(SIGINT, signalhandler);
    //首页
    face(IP, PORT);
    //主页
    home();
    
    return 0;
}
