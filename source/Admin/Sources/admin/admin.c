#include "admin.h"

//管理员登录页面
void admin_login()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                  chatroom管理员\n");
    printf("----------------------------------------------------------------------------------------\n"); 
    
    //输入管理员账号
    char account[16] = {0};
    printf("账号: ");
    while(1)
    {
        scanf("%s", account);
        if (strcmp(account, "admin") == 0)
        {
            break;
        }
        else
        {
            printf("管理员账号有误!\n请重新输入: ");
        }
    }
    //输入管理员密码
    char password[16] = {0};
    printf("密码: ");
    while (1)
    {
        scanf("%s", password);
        if(0 == strcmp(password, "admin"))
        {
            break;
        }
        else
        {
            printf("管理员密码有误!\n请重新输入: ");
        }
    }
}

//主页菜单
static void home_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------------------------------------------\n");
    printf("                                                    chatroom用户管理\n");
    printf("----------------------------------------------------------------------------------------------------------------------------\n"); 
    printf("                            1.查看所有用户  2.查看在线用户  3.禁言  4.解除禁言  5.踢人下线  0.退出\n");
    printf("----------------------------------------------------------------------------------------------------------------------------\n"); 
}
//消息接收和处理
static void msg_handler(const void *buf, int msg_size, int flags)
{
    //发送信息
    client_send(buf, msg_size);
    ser_msg_t ser_msg;
    if(0 == flags)
    {
        //接收信息
        client_recv(&ser_msg, sizeof(ser_msg_t));
        //处理信息
        printf("%s\n", ser_msg.reply);
    }
    else
    {
        while(1)
        {
            memset(&ser_msg, 0, sizeof(ser_msg_t));
            //接收信息
            client_recv(&ser_msg, sizeof(ser_msg_t));
            //处理信息
            printf("%s\n", ser_msg.reply);
            //-1: 操作有误  1: 后面还有信息   0: 最后一条系统信息
            if(-1 == ser_msg.ctl.cmd || 0 == ser_msg.ctl.cmd)
            {
                break;
            }
        }
    }
}
//查看所有用户
static void check_all()
{
    msg_sql_t admin_msg;
    admin_msg.ctl.cfd = -1;
    admin_msg.ctl.cmd = CHECK_ALL;
    admin_msg.ctl.msg_size = sizeof(msg_sql_t);
    strcpy(admin_msg.sql_buf,"select *from user;");

    //消息处理
    printf("账号\t\t昵称\t\t密码\t\t密保\t\t电话\t\t性别\t年龄\t生日\t\t禁言\tVIP\n");
    msg_handler(&admin_msg, sizeof(msg_sql_t), 1);
}
//shezhiID
static void set_id(char *id) 
{
    printf("账号: ");
    while (1)
    {
        scanf("%s", id);
        if(strlen(id) != 10)
        {
            printf("账号长度错误!\n");
        }
        else if(0 != strncmp("user", id, 4))
        {
            printf("账号格式错误!\n");
        }
        else
        {
            break;
        }
        printf("请重新输入: \n");
    }
}
//禁言与解除禁言
static void talk(int flags)
{
    char uid[11] = {0};
    set_id(uid);

    //封装信息
    msg_who_t ban_msg;
    ban_msg.ctl.cfd = -1;
    ban_msg.ctl.cmd = flags;
    ban_msg.ctl.msg_size = sizeof(msg_who_t);
    strcpy(ban_msg.who_id, uid);
    //封装sql语句
    int ban = (BAN == flags) ? 1 : 0;
    sprintf(ban_msg.sql_buf,"update user set ban = %d where uid = '%s';", ban, uid);
    //消息处理
    msg_handler(&ban_msg, sizeof(msg_who_t), 0);   
}
//查看在线用户
static void check_online()
{
    //封装信息
    ctl_t admin_msg;
    admin_msg.cfd = -1;
    admin_msg.cmd = CHECK_ONLINE;
    admin_msg.msg_size = sizeof(ctl_t);

    //消息处理
    msg_handler(&admin_msg, sizeof(ctl_t), 1);
}
//替人下线
static void remove_user()
{
    printf("请输入要踢下线的用户账号:\n");
    char uid[11] = {0};
    set_id(uid);

    msg_who_t rm_msg;
    rm_msg.ctl.cfd = -1;
    rm_msg.ctl.cmd = REMOVE_USER;
    rm_msg.ctl.msg_size = sizeof(msg_who_t);
    strcpy(rm_msg.who_id, uid);

    msg_handler(&rm_msg, sizeof(msg_who_t), 0);   
}
//主页功能函数
void home_func()
{
    home_menu();
    int choice;
    while(1)
    {
        printf(">>>: ");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1:                     //查看所有用户
                check_all();
                break;
            case 2:                     //查看在线用户
                check_online();     
                break;
            case 3:                     //禁言
                talk(BAN);
                break;
            case 4:                     //解除禁言
                talk(NOT_BAN);
                break;
            case 5:                     //踢人下线
                remove_user();
                break;
            case 0:                     //退出
                exit(0);
            default:
                home_menu();
                break;
        }
    }
}
