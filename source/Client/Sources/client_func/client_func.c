#include "client_func.h"

//保存用户个人信息
static info_t my_info;

//获取当前的日期和时间
static struct tm *get_time()
{
    time_t t;
    struct  tm *lt;
    time(&t);
    lt = localtime(&t);
    
    return lt;
}

//设置账号，在登录找会密码时输入账号调用，注册时不调用
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

//设置密码
static void set_password(char *password)
{
    printf("密码: ");
    while(1)
    {
        scanf("%s", password);
        if(strlen(password) < 10)
        {
            printf("密码过短!\n");
        }
        else if(strlen(password) > 20)
        {
            printf("密码过长!\n");
        }
        else if('0' <= password[0] && password[0] <= '9')
        {
            printf("密码首个字母不能为数字!\n");
        }
        else
        {
            break;
        }
        printf("请重新输入: ");
    }
}

//设置昵称
static void set_nickname(char *nickname)
{
    printf("昵称: ");
    while (1)
    {
        scanf("%s", nickname);
        if(strlen(nickname) > 28)
        {
            printf("你的昵称过长!\n请重新输入: \n");
        }
        else
        {
            break;
        }
    } 
}

//设置密码
static void set_reg_password(char *password)
{
    set_password(password);
    char confirm[21] = {0};    //确认密码
    printf("确认密码: ");
    while(1)
    {
        scanf("%s", confirm);
        if(0 == strcmp(password, confirm))
        {
            break;
        }
        else
        {
            printf("密码确认有误! \n请重新输入: ");
        }
    }
}

//设置密保
static void set_security(char *security)
{
    printf("密保: ");
    while(1)
    {
        scanf("%s", security);
        if(strlen(security) < 10)
        {
            printf("密保过短!\n");
        }
        else if (strlen(security) > 20)
        {
            printf("密保过长!\n");
        }
        
        else
        {
            break;
        }
        printf("请重新输入: ");
    }
}

//设置电话号码
static void set_telephone(char *tel)
{
    printf("电话号码: ");
    while(1)
    {
        scanf("%s", tel);
        if('1' != tel[0])
        {
            printf("电话号码格式有误!\n");
        }
        else if(strlen(tel) != 11)
        {
            printf("电话号码长度有误!\n");
        }
        else
        {
            break;
        }
        printf("请重新输入: ");
    }
}

//检查输入的生日天数是否正确
static int check_day_error(birthday_t *bday)
{
    int flag = 0;
    if(4 == bday->month || 6 == bday->month || 9 == bday->month || 10 == bday->month || 11 == bday->month)
    {
        //4、6、9、10、11月份只有30天
        if(bday->day > 30)
        {
            flag = -1;
        }
    }
    else if(2 == bday->month)
    {
        //闰年2月29天，平年28天
        if((0 == bday->year % 4 && 0 != bday->year % 100) || 0 == bday->year % 400)
        {
            if(bday->day > 29)
            {
                flag = -1;
            }
        }
        else
        {
            if(bday->day > 28)
            {
                flag = -1;
            }
        }
    }
    else
    {
        if(bday->day > 31)
        {
            flag = -1;
        }   
    }
    return flag;
}

//设置生日
static void set_birthday(birthday_t *bday)
{
    struct tm *now = get_time();
    printf("生日: ");
    scanf("%d%d%d", &bday->year, &bday->month, &bday->day);
    //检查年份
    while(bday->year > (now->tm_year + 1900))
    {
        printf("年份错误!请重新输入年份: \n");
        scanf("%d", &bday->year);
    }
    //检查月份
    while(bday->month < 1 || bday->month > 12)
    {
        printf("月份有误!请重新输入月份: \n");
        scanf("%d", &bday->month);
    }
    //检查天数
    while(bday->day < 1 || -1 == check_day_error(bday))
    {
        printf("天数有误!请重新输入天数: \n");
        scanf("%d", &bday->day);
    }
}

//设置性别
static void set_sex(char *sex)
{
    int flag = 1;   //默认为男
    printf("性别(1.男 0.女): ");
    scanf("%d", &flag);
    if(1 == flag)
    {
        strcpy(sex, "男");
    }
    else
    {
        strcpy(sex, "女");
    }
}

//根据生日自动计算周岁年龄
static void set_age(int *age,const birthday_t *bday)
{
    struct tm *now = get_time();
    *age = now->tm_year + 1900 - bday->year;
    if(now->tm_mon + 1 > bday->month)
    {
        return ;
    }
    else if (now->tm_mon + 1 < bday->month)
    {
        *age -= 1;
    }
    else
    {
        if(now->tm_mday >= bday->day)
        {
            return ;
        }
        else
        {
            *age -= 1;
        }
    }
}

//修改功能的菜单
static void modify_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                     修改个人信息\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                      1.昵称 2.密码 3.密保 4.号码 5.性别 6.生日 0.返回\n");
    printf("----------------------------------------------------------------------------------------\n");
}

//修改功能的操作函数 ：flags==UPDATE即需发送到服务器
static void modify_func(int flags)      //这里没有使用switch,而是使用的函数指针数组
{
    modify_menu();
    int choice;
    while(1)
    {
        scanf("%d", &choice);
        if(0 == choice) 
        {
            break;
        } 
        else if(6 == choice)    //修改生日
        {
            set_birthday(&my_info.bday);
            set_age(&my_info.age, &my_info.bday);
        }
        else if(choice > 6 || choice < 0)
        {
            modify_menu();
        }
        else   //修改昵称、密码、密保、电话、性别
        {
            choice -= 1;
            char *my_buf[5] = {my_info.nickname, my_info.password, my_info.security, my_info.tel, my_info.sex};
            void (*set_func[5])(char *buf) = {set_nickname, set_password, set_security, set_telephone, set_sex};
            set_func[choice](my_buf[choice]);    
        }
        if(UPDATE == flags) //同步到服务器
        {
            char *my_buf[5] = {my_info.nickname, my_info.password, my_info.security, my_info.tel, my_info.sex};
            char *buf[5] = {"nickname", "password", "security", "tel", "sex"};
            msg_sql_t msg_sql;
            msg_sql.ctl.cfd = -1;
            msg_sql.ctl.cmd = UPDATE;
            msg_sql.ctl.msg_size = sizeof(msg_sql_t);
            //封装sql语句
            if(6 == choice)
            {
                sprintf(msg_sql.sql_buf, "update user set age = '%d', year = '%d', month = '%d', day = '%d' where uid = '%s';", my_info.age, my_info.bday.year, my_info.bday.month, my_info.bday.day, my_info.myid);
            }
            else
            {
                sprintf(msg_sql.sql_buf, "update user set %s = '%s' where uid = '%s';", buf[choice], my_buf[choice], my_info.myid);
            }
            client_send(&msg_sql, sizeof(msg_sql_t));
        }
    } 
}

//打印用户信息(全局变量my_info)
static void print_myinfo()
{
    printf("你的信息如下:\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("            昵称:   %s\n", my_info.nickname);
    printf("            密码:   %s\n", my_info.password);
    printf("            密保:   %s\n", my_info.security);
    printf("            年龄:   %d\n", my_info.age);
    printf("            性别:   %s\n", my_info.sex);
    printf("            生日:   %d/%d/%d\n", my_info.bday.year, my_info.bday.month, my_info.bday.day);
    printf("        电话号码:   %s\n", my_info.tel);
    printf("----------------------------------------------------------------------------------------\n");
}

/*****************************************************首页操作*********************************************************/
//注册信息
static msg_sql_t client_register()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                  欢迎注册chatroom\n");
    printf("    温馨提示:\n");
    printf("            1、账号由chatroom服务端分配,不用输入,请牢记自己的账号密码\n");
    printf("            2、密码密保最少10位最多20位,密码由字母下划线数字组成且不能数字开头\n");
    printf("            3、注册成功后要再重新登录\n");
    printf("            4、注册前请认真阅读使用协议\n");
    printf("----------------------------------------------------------------------------------------\n");
    //注册信息
    memset(&my_info, 0, sizeof(info_t));
    printf("请输入信息完成注册:\n");
    strcpy(my_info.myid, "0000000000");         //id还未分配使用0占位
    set_nickname(my_info.nickname);
    set_reg_password(my_info.password);
    set_security(my_info.security);
    set_sex(my_info.sex);
    set_birthday(&my_info.bday);
    set_telephone(my_info.tel);
    set_age(&my_info.age, &my_info.bday);
    //检查注册信息、确认无误即发送
    while(1)
    {
        print_myinfo();

        printf("\n确认信息是否有误,无误即发送(1.有 0.无): ");
        int confirm;
        scanf("%d", &confirm);
        if(0 == confirm)    //信息无误，发送至服务器注册
        {
            msg_sql_t reg;
            //设置注册的请求信息
            reg.ctl.cmd = REGISTER;                     //注册
            reg.ctl.msg_size = sizeof(msg_sql_t);       //消息大小
            reg.ctl.cfd = -1;                           //客户端置-1,发送过去服务器设置
            //封装sql语句
            sprintf(reg.sql_buf, "insert into user values('%s', '%s', '%s', '%s','%s', '%s', %d, %d, %d, %d, %d, %d);", my_info.myid, my_info.nickname, my_info.password, my_info.security, my_info.tel, my_info.sex, my_info.age, my_info.bday.year, my_info.bday.month, my_info.bday.day, 0, 0);
            //发送注册请求 
            return reg;
        }
        else    //信息有误再修改
        {
            modify_func(REGISTER); 
        }
    }
}

//登录
static msg_sql_t client_login()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                  欢迎登录chatroom\n");
    printf("----------------------------------------------------------------------------------------\n");
    
    char id[11] = {0};
    char password[21] = {0};
    set_id(id);
    set_password(password);

    int confirm;
    printf("\n确认登录(0.登录 1.不登录):   ");
    scanf("%d", &confirm);
    if(0 == confirm)      //确认登陆
    {
        msg_sql_t login;
        login.ctl.cmd = LOGIN;                      //设置登录操作
        login.ctl.msg_size = sizeof(msg_sql_t);     //设置消息大小
        login.ctl.cfd = -1;
        //封装登录的sql语句 -- select * from user where uid = '' and password = ''
        sprintf(login.sql_buf, "select *from user where uid = '%s' and password = '%s';", id, password);
        return login;
    }
    else    //取消操作，退出
    {
        system("clear");
        exit(0);
    }
}

//找回密码
static msg_sql_t client_find_psd()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                  找回chatroom账号\n");
    printf("----------------------------------------------------------------------------------------\n"); 
    
    char id[11] = {0};
    char security[21] = {0};
    set_id(id);
    set_security(security);

    int confirm;
    printf("\n确认发送(0.发送 1.不发送):   ");
    scanf("%d", &confirm);
    if(0 == confirm)       //确认发送
    {
        msg_sql_t find_psd;
        find_psd.ctl.cfd = -1;
        find_psd.ctl.cmd = FIND_PASSWORD;
        find_psd.ctl.msg_size = sizeof(msg_sql_t);
        //封装找回密码的sql语句 
        sprintf(find_psd.sql_buf, "select password from user where uid = '%s' and security = '%s';", id, security);
        return find_psd;
    }
    else    //取消操作，退出
    {
        system("clear");
        exit(0);
    }
}

//首页菜单
static void face_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                  欢迎使用chatroom\n\n");
    printf("                         1.登录  2.注册  3.忘记密码  0.退出\n");
    printf("----------------------------------------------------------------------------------------\n"); 
}

//首页操作函数
static msg_sql_t face_func()
{
    int choice;
    msg_sql_t msg_tmp = {0};
    while(1)
    {
        printf(">>>: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:             //登录
                msg_tmp = client_login();
                break;
            case 2:             //注册
                msg_tmp = client_register();
                break;
            case 3:             //忘记密码
                msg_tmp = client_find_psd();
                break;
            case 0:             //退出首页
                exit(0);
                break;
            default:
                printf("指令有误! 请重新输入!\n");
                continue;
        }
        break;
    }
    return msg_tmp;
}

//首页程序
void face(const char *ip, int port)
{
    ser_msg_t ser_msg;
    face_menu();

    //获取首页消息
    msg_sql_t msg = face_func();
    //连接服务器
    client_connect(ip, port);
    while(1)
    {
        //发送消息
        client_send(&msg, sizeof(msg_sql_t));
        //接收消息
        memset(&ser_msg, 0, sizeof(ser_msg_t));
        client_recv(&ser_msg, sizeof(ser_msg_t));
        
        if(LOGIN == ser_msg.ctl.cmd)    //登录成功进入主页
        {
            memcpy(&my_info, ser_msg.reply, sizeof(info_t));
            return ;
        }
        else
        {
            face_menu();
            if(OPT_ERR == ser_msg.ctl.cmd)  //操作失败
            {
                printf("%s\n", ser_msg.reply);
            }
            else if(REGISTER == ser_msg.ctl.cmd)    //注册成功要再登录
            { 
                printf("你的账号为: %s  可以登录了!!\n", ser_msg.reply);
            }
            else if(FIND_PASSWORD == ser_msg.ctl.cmd)   //找回密码成功也要再登录
            {
                printf("你的密码为: %s  可以登录了!!\n", ser_msg.reply);
            }
        }
        memset(&msg, 0, sizeof(msg_sql_t));
        msg = face_func();    
    }  
}

/*****************************************************主页操作*********************************************************/

//创建保存消息和文件的目录
static void folder_create()
{
    char buf[100] = {0};
    sprintf(buf, "Record/%s", my_info.myid);
    //先判断是否有保存信息的文件夹
    if(access(buf, F_OK) == 0)  //文件存在
    {
        return;
    }
    //创建主文件夹，使用用户ID命名
    sprintf(buf, "mkdir Record/%s", my_info.myid);
    system(buf);    
    //创建自己文件: chat和 file
    sprintf(buf, "mkdir Record/%s/chat Record/%s/file", my_info.myid, my_info.myid);
    system(buf);
}

//聊天功能的菜单
static void chat_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                       chatroom聊天\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                    1.群聊  2.私聊  3.留言  4.我的留言  5.查看在线  0.返回\n");
    printf("----------------------------------------------------------------------------------------\n"); 
}
//用户发送消息, flags标志是群发（GROUP_CHAT）私聊（PRIVATE_CHAT）留言（LEAVE_WORD）
static void send_msg(const char *who, int flags)
{
    msg_chat_t msg_chat;
    msg_chat.ctl.cfd = -1;
    msg_chat.ctl.cmd = flags;
    msg_chat.ctl.msg_size = sizeof(msg_chat_t);
    memset(msg_chat.from_id, 0, sizeof(msg_chat.from_id));
    memset(msg_chat.from_name, 0, sizeof(msg_chat.from_name));
    strcpy(msg_chat.to_id, who);

    char chat_path[128] = {0};
    if(PRIVATE_CHAT == flags)     //私聊
    {
        //聊天记录保存: user000000发送给user000001的消息 -- Record/user000000/chat/user000001.txt
        sprintf(chat_path, "Record/%s/chat/%s.txt", my_info.myid, who);
    }
    else if(GROUP_CAHT == flags)    //群聊
    {
        //聊天记录保存: group_chat.txt
        sprintf(chat_path, "Record/%s/chat/group_chat.txt", my_info.myid);
    }
    else    //留言
    {
        //留言保存: leave_word.txt
        sprintf(chat_path, "Record/%s/chat/leave_word.txt", my_info.myid);
    }
    FILE *fp = myfopen(chat_path, "a");
    printf("键入(bye)结束本次消息发送!\n");
    while (1)
    {
        memset(msg_chat.chat_buf, 0, sizeof(msg_chat.chat_buf));
        scanf("%s", msg_chat.chat_buf);
        if(0 == strcmp("bye", msg_chat.chat_buf))       //bye: 退出本次聊天的结束语
        {
            break;
        }
        if(LEAVE_WORD == flags)
        {
            //留言保存格式
            fprintf(fp, "我给(%s)的留言:  %s\n", who, msg_chat.chat_buf);
        }
        else
        {
            //私聊群发保存格式
            fprintf(fp, "我发的消息:  %s\n",msg_chat.chat_buf);
        }
        //发送信息
        client_send(&msg_chat, sizeof(msg_chat_t));
    }
    myfclose(fp);
}
//查看留言
static void look_word_func()
{
    msg_sql_t msg_sql;
    msg_sql.ctl.cmd = LOOK_WORD;
    msg_sql.ctl.cfd = -1;
    msg_sql.ctl.msg_size = sizeof(msg_sql_t);
    //封装参看留言的sql语句
    sprintf(msg_sql.sql_buf, "select *from lewd where uid = '%s';", my_info.myid);
    client_send(&msg_sql, sizeof(msg_sql_t));
}
//查看在线用户
static void check_online_func()
{
    //封装信息
    ctl_t user_check;
    user_check.cfd = -1;
    user_check.cmd = CHECK_ONLINE;
    user_check.msg_size = sizeof(ctl_t);

    client_send(&user_check, sizeof(ctl_t));
}
//聊天功能的函数
static void chat_func()
{
    chat_menu();
    int choice;
    while(1)
    {    
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:                 //群聊
                send_msg("群聊", GROUP_CAHT);
                break;
            case 2:                //私聊
            {
                printf("请输入对方的");
                char who[11] = {0};
                set_id(who); 
                send_msg(who, PRIVATE_CHAT);
                break;
            }
            case 3:                 //留言
            {
                printf("请输入对方的");
                char who[11] = {0};
                set_id(who);
                send_msg(who, LEAVE_WORD);
                break;
            }
            case 4:                 //查看新的留言
                look_word_func();
                break;
            case 5:                 //查看在线用户
                check_online_func();
                break;
            case 0:
                return;
            default:
                chat_menu();
                break;
        }
        if(choice < 4)
        {
            printf("\n键入数字(>5)清屏显示菜单或0直接返回: ");
        }
    }
}

//文件功能的菜单
static void file_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                       chatroom文件\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                         1.查看群文件  2.下载文件  3.上传文件  0.返回\n");
    printf("----------------------------------------------------------------------------------------\n"); 
}

//查看群文件
static void check_file_func()
{
    ctl_t check_file;
    check_file.cfd = -1;
    check_file.cmd = CHECK_FILE;
    check_file.msg_size = sizeof(ctl_t);

    client_send(&check_file, sizeof(ctl_t));
}
//上传文件
static void upload_file_func()
{
    printf("请输入文件名: ");
    char filename[32] = {0};
    scanf("%s", filename);
    printf("文件路径(0代表默认路径): ");
    char file[256] = {0};
    scanf("%s", file);
    if(strcmp("0", file) == 0)     //默认路径
    {
        sprintf(file, "Record/%s/file/", my_info.myid);
    }
    strcat(file, filename);

    if(access(file, F_OK) != 0)     //检查文件是否存在
    {
        printf("文件不存在!\n");
        return;
    }
    printf("正在上传文件: %s\n", file);
    FILE *fp = fopen(file, "r");
    if(NULL == fopen)
    {
        perror("uploard fopen");
        return;
    }
   
    msg_file_t msg_file;
    msg_file.ctl.cfd = -1;
    msg_file.ctl.cmd = UPLOAD_FILE;
    msg_file.ctl.msg_size = sizeof(msg_file_t);
    strcpy(msg_file.filename, filename);
    strcpy(msg_file.from_id, my_info.myid);
    //获取文件字节数
    struct stat mystat;
    stat(file, &mystat); 
    msg_file.file_size = mystat.st_size;
    while(1)
    {
        if(feof(fp))
        {
            break;
        }
        msg_file.flags = 1;
        int len = fread(msg_file.file_buf, 1, sizeof(msg_file.file_buf) - 1, fp);
        client_send(&msg_file, 2000);
        memset(msg_file.file_buf, 0, sizeof(msg_file.file_buf));
        usleep(100000);
    }

    myfclose(fp);
}
//下载文件
static void download_file_func()
{
    msg_file_t msg_file;
    msg_file.ctl.cfd = -1;
    msg_file.ctl.cmd = DOWNLOAD_FILE;
    msg_file.ctl.msg_size = sizeof(msg_file_t);
    printf("请输入下载的文件名: ");
    scanf("%s", msg_file.filename);
    printf("正在下载群文件: %s\n", msg_file.filename);
    client_send(&msg_file, sizeof(msg_file_t));
}
//文件功能的函数
static void file_func()
{
    file_menu();
    int choice;
    while(1)
    {
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:                 //查看群文件
                check_file_func();
                break;
            case 2:                //下载群文件
                download_file_func();
                break;
            case 3:                 //上传文件
                upload_file_func();
                break;
            case 0:                //返回
                return;
            default:               //其他指令显示菜单
                file_menu();
                break;
        }
    }
}

//个人管理功能的菜单
static void myself_menu()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                      chatroom个人管理\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                          1.查看个人信息  2.修改个人信息  3.开通VIP\n");
    printf("                          4.查看聊天记录  5.删除聊天记录  0.返回\n");
    printf("----------------------------------------------------------------------------------------\n"); 
}

//历史聊天记录管理： 查看（1）与删除（0）
static void chat_record_func(int flags)
{
    int choice;
    char chat_path[128] = {0};
    char cmd[150] = {0};
    while(1)
    {
        printf("历史聊天记录(1.群聊 2.私聊 3.留言 0.退出): ");
        scanf("%d", &choice);
        sprintf(chat_path, "Record/%s/chat/", my_info.myid);
        if(1 == choice)
        {
            strcat(chat_path, "group_chat.txt");
        }
        else if (2 == choice)
        {   
            char uid[11] = {0};
            printf("请输入用户");
            set_id(uid);
            strcat(chat_path, uid);
            strcat(chat_path, ".txt");
        }
        else if(3 == choice)
        {
            strcat(chat_path, "leave_word.txt");
        }
        else
        {
            break;
        }
        //聊天记录不存在
        if(access(chat_path, F_OK) != 0)
        {
            printf("无历史该聊天记录: %s\n", chat_path);
            continue;
        }
        //执行命令
        if(1 == flags)      //查看
        {   
            sprintf(cmd, "%s %s", "cat", chat_path);
        }
        else    //删除
        {
            sprintf(cmd, "%s %s", "rm", chat_path);
        }
        system(cmd);
        memset(chat_path, 0, sizeof(chat_path));
        memset(cmd, 0, sizeof(cmd));
    }
}

//开通VIP
static void become_vip()
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                         开通VIP\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("    VIP特权:  可以使用留言等功能!\n");
    printf("----------------------------------------------------------------------------------------\n"); 
    
    int become;
    printf("chatroom用户VIP: 5元/月  是否开通(0.否 1.是): ");
    scanf("%d", &become);
    if(0 == become)
    {
        return;
    }
    else if(1 == become)    //开通VIP
    {
        printf("输入支付金额: ");
        int money;
        scanf("%d", &money);
        if(5 == money)
        {
            printf("正在支付中......\n");
            sleep(1);
            
            msg_sql_t msg_sql;
            msg_sql.ctl.cfd = -1;
            msg_sql.ctl.cmd = UPDATE;
            msg_sql.ctl.msg_size = sizeof(msg_sql_t);
            sprintf(msg_sql.sql_buf, "update user set vip = 1 where uid = '%s';", my_info.myid);
            client_send(&msg_sql, sizeof(msg_sql_t));
            usleep(100000);
        }
        else
        {
            printf("支付失败!\n");
        }
    }
}

//个人管理功能的操作函数
static void myself_func()
{
    myself_menu();
    int choice;
    while(1)
    {    
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:             //查看个人信息
                print_myinfo();
                break;
            case 2:             //修改个人信息
                modify_func(UPDATE);
                break;
            case 3:             //开通VIP
                become_vip();
                break;
            case 4:             //查看历史聊天记录
                chat_record_func(1);
                break;
            case 5:             //删除历史聊天记录
                chat_record_func(0);
                break;
            case 0:             //返回
                return;
            default:            //错误指令显示菜单
                myself_menu();
                continue;
        }
        if(choice < 6)
        {
            printf("\n键入数字(>5)清屏显示菜单或0直接返回: ");
        }   
    }
}

//主页菜单
static void home_menu()    
{
    system("clear");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                                       chatroom\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("                            1.聊天  2.文件  3.个人  0.退出\n");
    printf("----------------------------------------------------------------------------------------\n"); 
}

//次线程入口函数:   接收并处理消息
static void *msg_routine(void *arg)
{
    char chat_buf[2000] = {0};
    char chat_path[128] = {0};
    while (1)  
    {
        memset(chat_buf, 0, sizeof(chat_buf));
        client_recv(chat_buf, sizeof(chat_buf));

        ctl_t *ctl = (ctl_t *)chat_buf;

        if(PRIVATE_CHAT == ctl->cmd || GROUP_CAHT == ctl->cmd)      //私聊和群聊
        {
            msg_chat_t *msg_chat = (msg_chat_t *)chat_buf;
            if(PRIVATE_CHAT == msg_chat->ctl.cmd)     //私聊
            {
                //聊天记录保存
                sprintf(chat_path, "Record/%s/chat/%s.txt",  my_info.myid, msg_chat->from_id);
                printf("%s(%s): (私聊)\n   %s\n", msg_chat->from_name, msg_chat->from_id, msg_chat->chat_buf);
            }
            else    //群聊
            {
                //聊天记录保存
                sprintf(chat_path, "Record/%s/chat/group_chat.txt",  my_info.myid);
                printf("%s(%s): (群聊)\n   %s\n", msg_chat->from_name, msg_chat->from_id, msg_chat->chat_buf);
            }
            FILE *fp = myfopen(chat_path, "a");
            fprintf(fp, "%s(%s):  %s\n", msg_chat->from_name, msg_chat->from_id, msg_chat->chat_buf);
            myfclose(fp);
        }
        else if(LOOK_WORD == ctl->cmd)    //留言
        {
            ser_msg_t *ser_msg = (ser_msg_t *)chat_buf;
            char word_path[100] = {0};
            sprintf(word_path, "Record/%s/chat/leave_word.txt", my_info.myid);
            FILE *fp = myfopen(word_path, "a");
            //处理信息
            printf("%s\n", ser_msg->reply);
            fprintf(fp, "%s\n", ser_msg->reply);   
            myfclose(fp);
        }
        else if(DOWNLOAD_FILE == ctl->cmd)       //下载文件
        {
            msg_file_t *msg_file = (msg_file_t *)chat_buf;
            char file[256] = {0};
            sprintf(file, "Record/%s/file/%s", my_info.myid, msg_file->filename);
            FILE *fp = fopen(file, "a+");
            if(NULL == fp)
            {
                perror("fopen");
                continue;
            }
            fwrite(msg_file->file_buf, 1, strlen(msg_file->file_buf), fp);  
            fclose(fp);
        }
        else        //其他不需处理的消息
        {
            ser_msg_t *ser_msg = (ser_msg_t *)chat_buf;
            printf("%s\n", ser_msg->reply);
            if(ser_msg->ctl.cmd == REMOVE_USER)     //被管理员踢下线
            {
                exit(0);
            }
        }  
    } 
}

//主页操作函数
void home()
{
    folder_create();    //创建消息记录目录
    //创建次线程
    pthread_t msg_tid;
    if(0 != pthread_create(&msg_tid, NULL, msg_routine, NULL))
    {
        perror("pthread_create error\n");
        exit(0);
    }
    
    int chioce;
    while(1)
    {
        home_menu();
        scanf("%d", &chioce);
        switch (chioce)
        {
            case 1:                     //聊天
                chat_func();
                break;
            case 2:                     //文件
                file_func();
                break;
            case 3:                     //个人
                myself_func();
                break;
            case 0:                     //退出软件
                pthread_cancel(msg_tid);
                pthread_join(msg_tid, NULL);
                exit(0);
                break;
            default:
                home_menu();
                continue;
        }
    }
}
