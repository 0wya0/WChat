#include "server_func.h"

/****************************创建id的API************************/
static struct
{
    int lid_num;    //logout表可用的账号数
    int uid_max;    //现在最大账号
}id_counter = {0, 0};
//创建一个账号
static int user_id_create(char *id)
{
    int id_tmp;
    if(0 == id_counter.lid_num)      //创建一个新的账号
    {
        id_tmp = id_counter.uid_max;
    }
    else    //从logout表中读取一个
    {
        mysqlite_getdata("select lid from logout where seq = 2;", &id_tmp, LOGOUT);
    }
    strcpy(id, "user000000");                       //user000000为模板
    char id_buf[7] = {0};
    sprintf(id_buf, "%d", id_tmp);                  //整型数转字符串
    int len = strlen(id_buf);
    strncpy(id + 10 - len, id_buf, len);            //插入对应的位置 
    //返回值为该账号的创建使用的序号
    return id_tmp;
}
//打开数据库并初始化id_counter
void id_init(void)
{
    //打开数据库
    mysqlite_open();
    //初始化id_counter
    mysqlite_getdata("select lid from logout where seq = 0;", &id_counter.lid_num, LOGOUT);
    mysqlite_getdata("select lid from logout where seq = 1;", &id_counter.uid_max, LOGOUT);
    printf("lid_num = %d\n", id_counter.lid_num);
    printf("uid_max = %d\n", id_counter.uid_max);
}
//将id_counter保存到logout,关闭数据库
void id_destroy(void)
{
    //将id_counter保存到logout
    char sql_buf[64];
    //将logout表可用的账号数存在seq = 0的lid
    sprintf(sql_buf, "update logout set lid = %d where seq = 0;", id_counter.lid_num);
    mysqlite_exec(sql_buf);
    //现在最大账号存在seq = 1的lid
    sprintf(sql_buf, "update logout set lid = %d where seq = 1;", id_counter.uid_max);
    mysqlite_exec(sql_buf);
    //关闭数据库
    mysqlite_close();
}

/****************************客户端处理函数***********************/
//注册消息处理函数
static void reg_handler(msg_sql_t *reg_msg)
{
    if(strlen(reg_msg->sql_buf) <= 0)
    {
        return ;
    }
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);

    char uid[11] = {0};
    int id_tmp = user_id_create(uid);        //创建一个账号
    //完善注册的sql语句
    strncpy(reg_msg->sql_buf + 25, uid, 10);
    //执行sql语句,成功则代表注册成功
    if(SQL_SUCCESS == mysqlite_exec(reg_msg->sql_buf))      //成功
    {
        //修改id_counter
        if(id_tmp == id_counter.uid_max)
        {
            id_counter.uid_max++;
        }
        else
        {
            char sql_buf[100];
            sprintf(sql_buf, "delete from logout where lid = %d;", id_tmp);
            mysqlite_exec(sql_buf);
            mysqlite_exec("update logout set seq = seq - 1 where seq > 1;");
            id_counter.lid_num--;
        }
        //通知客户端注册成功
        ser_msg.ctl.cmd = REGISTER;
        strcpy(ser_msg.reply, uid);
    }
    else    //失败
    {
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "注册失败");  
    }
    send_msg(reg_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
}
//登录消息处理函数
static void login_handler(msg_sql_t *login_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    //用于保存查询到的结果
    info_t my_info;
    char age[4] = {0};
    char year[6] = {0};
    char month[4] = {0};
    char day[4] = {0};
    char vip[3] = {0};
    char ban[3] = {0};
    char *data[12] = {my_info.myid, my_info.nickname, my_info.password, my_info.security, my_info.tel, my_info.sex, age, year, month, day, ban, vip};
    //查询user表判断账户密码是否正确
    if(SQL_SUCCESS == mysqlite_getdata(login_msg->sql_buf, data, ONLINE) && 10 == strlen(my_info.myid))
    {
        //检测重复登陆，不能重复登录
        if(NULL == ol_query_byid(my_info.myid))
        {
            my_info.age = atoi(age);
            my_info.bday.year = atoi(year);
            my_info.bday.month = atoi(month);
            my_info.bday.day = atoi(day);
            //将客户端cfd添加到在线用户链表
            ol_insert_head(login_msg->ctl.cfd, my_info.myid, my_info.nickname, atoi(ban), atoi(vip));
            //通知客户端登录成功
            ser_msg.ctl.cmd = LOGIN;
            memcpy(ser_msg.reply, &my_info, sizeof(info_t));
        }
        else
        {
            ser_msg.ctl.cmd = OPT_ERR;
            strcpy(ser_msg.reply, "你的账号已登陆! 不能重复登录!");
        }
    }
    else
    {
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "登录失败!账号或密码错误!");
    }
    send_msg(login_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
}
//找回密码消息处理函数
static void find_psd_handler(msg_sql_t *find_psd_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    //用于保存查询到的的密码
    char password[21] = {0};
    if(SQL_SUCCESS == mysqlite_getdata(find_psd_msg->sql_buf, password, FIND) && 0 != strlen(password))
    {
        ser_msg.ctl.cmd = FIND_PASSWORD;
        strcpy(ser_msg.reply,password);
    }
    else
    {
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "密码找回失败!账号或密保错误!");
    }
    send_msg(find_psd_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
}

//聊天消息转发与保存
static void chat_handler(msg_chat_t *chat_msg)  
{
    //发送给所有的在线用户
    printf("chat = %s\n", chat_msg->chat_buf);
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.cmd = OPT_ERR;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);

    //查看用户是否被禁言
    online_t *from = ol_query_bycfd(chat_msg->ctl.cfd);
    if(1 == from->ban)       //用户被禁言
    {
        strcpy(ser_msg.reply, "你已被管理员禁言");
        send_msg(chat_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
        return ;
    }

    //转发
    strcpy(chat_msg->from_id, from->id);
    strcpy(chat_msg->from_name, from->nickname);

    if(chat_msg->ctl.cmd == GROUP_CAHT)         //群聊
    {
        //发送给所有的在线用户
        ol_t *ol = get_ol();
        online_t *p = ol->ol_head->next;
        while (p)
        {
            if(from == p)   //自己
            {
                p = p->next;
                continue;
            }
            send_msg(p->cfd, chat_msg, sizeof(msg_chat_t));
            p = p->next;
        }
        //将用户消息保存:
        FILE *fp_g = myfopen("Record/chat/group_chat.txt", "a");
        fprintf(fp_g, "%s(%s): %s\n", chat_msg->from_name, chat_msg->from_id, chat_msg->chat_buf);    
        myfclose(fp_g);
    }
    else   //私聊
    {
        //转发消息
        online_t *to = ol_query_byid(chat_msg->to_id);
        if(NULL == to)
        {
            strcpy(ser_msg.reply, "对方不在线!");
            send_msg(chat_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
            return ;
        }
        send_msg(to->cfd, chat_msg, sizeof(msg_chat_t));
        //私聊
        FILE *fp_p = myfopen("Record/chat/private_chat.txt", "a");
        fprintf(fp_p, "From(%s)to(%s): %s\n",chat_msg->from_id, chat_msg->to_id, chat_msg->chat_buf);    
        myfclose(fp_p);
    }
}

//留言
static void leave_word_handler(msg_chat_t *chat_msg)
{
    printf("接收到留言!\n");
    ser_msg_t ser_msg;
    ser_msg.ctl.cmd = OPT_ERR;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    //查看用户是否被禁言
    online_t *from = ol_query_bycfd(chat_msg->ctl.cfd);
    if(0 == from->vip)       //用户被禁言
    {
        strcpy(ser_msg.reply, "你还不是VIP!留言功能需要你成为VIP!");
        send_msg(chat_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
        return ;
    }

    char sql_buf[2000] = {0};
    sprintf(sql_buf, "insert into lewd values('%s', '%s', '%s', '%s');", chat_msg->to_id, from->nickname, from->id, chat_msg->chat_buf);
    if(SQL_FAILURE == mysqlite_exec(sql_buf))
    {
        strcpy(ser_msg.reply, "留言失败!");
        send_msg(chat_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
    }
    else
    {
        strcpy(ser_msg.reply, "留言成功!");
        send_msg(chat_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
    }
}
//查看留言
static void look_word_handler(msg_sql_t *look_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.cmd = OPT_ERR;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    //查看用户是否被禁言
    online_t *from = ol_query_bycfd(look_msg->ctl.cfd);

    char **result;
    int row, col;
    int ret = mysqlite_get_table(look_msg->sql_buf, &result, &row, &col);
    if(SQL_FAILURE == ret)
    {
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "参看留言失败!");   
        send_msg(look_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
    }
    else
    {
        printf("row = %d  col = %d\n", row, col);
        if(0 == row)
        {
            ser_msg.ctl.cmd = 0;
            strcpy(ser_msg.reply, "你暂时没有新的留言!");
            send_msg(look_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
        }
        else
        {
            //发送并保存留言
            FILE *fp = myfopen("Record/chat/leave_word.txt", "a");
            int i;
            for(i = 1; i <= row; i++)
            {
                ser_msg.ctl.cmd = LOOK_WORD;
                //uid: result[i*col+0] , fname: result[i*col+1] , fid: result[i*col+2] , word: result[i*col+4]
                sprintf(ser_msg.reply, "%s(%s):  %s",result[i*col+1],result[i*col+2],result[i*col+3]);
                fprintf(fp, "(%s)给(%s)的留言: %s\n", result[i*col+2], result[i*col+0], result[i*col+3]);
                send_msg(look_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
                memset(&ser_msg.reply, 0, sizeof(ser_msg_t));
                usleep(10000);
            }
            ser_msg.ctl.cmd = 0;
            sprintf(ser_msg.reply, "共有%d条最新留言!", row);
            send_msg(look_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
            myfclose(fp);

            //删除lewd表中该联系人的留言
            char sql_buf[64] = {0};
            sprintf(sql_buf, "delete from lewd where uid = '%s';", result[col]);
            mysqlite_exec(sql_buf);
        }
    }
}

//开通VIP, 修改信息
static void update_handler(msg_sql_t *update_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    if(SQL_FAILURE == mysqlite_exec(update_msg->sql_buf))
    {
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "操作失败!");
    }
    else
    {
        //更新用户在线链表
        online_t *p = ol_query_bycfd(update_msg->ctl.cfd);
        char sql_buf[128] = {0};
        sprintf(sql_buf, "select nickname, vip from user where uid = '%s';", p->id);
        char vip[3] = {0};
        char *data[2] = {p->nickname, vip};
        mysqlite_getdata(sql_buf, data, ONLINE);
        p->vip = atoi(vip);

        ser_msg.ctl.cmd = UPDATE;
        strcpy(ser_msg.reply, "操作成功!");
    }
    send_msg(update_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
}

//查看群文件
static void check_file_handler(ctl_t *ctl)
{
    system("ls Record/file > Record/files.txt");
    FILE *fp = myfopen("Record/files.txt", "r");
    ser_msg_t ser_msg;
    ser_msg.ctl.cfd = -1;
    ser_msg.ctl.cmd = OPT_ERR;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    if(NULL == fp)
    {
        strcpy(ser_msg.reply, "查看失败!");
        send_msg(ctl->cfd, &ser_msg, sizeof(ser_msg_t));

        return;
    }
    char buf[64] = {0};       //默认文件名不超过64个字节
    int num = sizeof(ser_msg.reply) / sizeof(buf), i = 0, count = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        if(i < num)
        {
            strcat(ser_msg.reply, buf);
            i++;
        }
        else
        {
            send_msg(ctl->cfd, &ser_msg, sizeof(ser_msg_t));
            memset(&ser_msg.reply, 0, sizeof(ser_msg.reply));
            i = 0;
            strcat(ser_msg.reply, buf);
            usleep(100000);
        }
        count++;
    }
    if(0 == count)
    {
        strcpy(ser_msg.reply, "暂无群文件!!");
    }
    if(i > 0)
    {
        send_msg(ctl->cfd, &ser_msg, sizeof(ser_msg_t));
        memset(&ser_msg.reply, 0, sizeof(ser_msg.reply));
        usleep(100000);
        sprintf(ser_msg.reply, "共%d个文件!",count);
    }
    send_msg(ctl->cfd, &ser_msg, sizeof(ser_msg_t));
    myfclose(fp);
}
//下载文件
static void download_file_handler(msg_file_t *msg_down)
{
    char file[256] = {0};
    sprintf(file, "Record/file/%s", msg_down->filename);
    printf("file = %s\n", file);
    //检查文件是否存在
    if(access(file, F_OK) != 0)
    {
        ser_msg_t ser_msg;
        ser_msg.ctl.cmd = OPT_ERR;
        strcpy(ser_msg.reply, "文件不存在!下载失败!");
        send_msg(msg_down->ctl.cfd, &ser_msg, sizeof(ser_msg_t));

        return;
    }
    //存在发送文件
    FILE *fp = myfopen(file, "r");
    msg_file_t msg_file;
    msg_file.ctl.cfd = -1;
    msg_file.ctl.cmd = DOWNLOAD_FILE;
    msg_file.ctl.msg_size = sizeof(msg_file_t);
    strcpy(msg_file.filename, msg_down->filename);
    struct stat mystat;
    stat(file, &mystat); 
    msg_file.file_size = mystat.st_size;
    while (1)
    {
        if(feof(fp))
        {
            break;
        }
        myfread(fp, msg_file.file_buf, sizeof(msg_file.file_buf) - 1);
        send_msg(msg_down->ctl.cfd, &msg_file, 2000);
        memset(msg_file.file_buf, 0, sizeof(msg_file.file_buf));
        usleep(100000);     //睡眠0.1秒
    }
    myfclose(fp);
}
//上传群文件
static void upload_file_handler(msg_file_t *msg_file)
{
    char file[256] = {0};
    sprintf(file, "Record/file/%s", msg_file->filename);
    FILE *fp = fopen(file, "a+");
    if(NULL == fp)
    {
        perror("upload_file_handler fopen");
        return ;
    }
    fwrite(msg_file->file_buf, 1, strlen(msg_file->file_buf), fp);  

    // if(ftell(fp) == msg_file->file_size)
    // {
    //     ser_msg_t ser_msg;
    //     ser_msg.ctl.cfd = -1;
    //     ser_msg.ctl.cmd = OPT_ERR;
    //     ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    //     strcpy(ser_msg.reply, "上传成功!");

    //     send_msg(msg_file->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
    // }
    fclose(fp);
}

//查看在线用户
static void check_online_handler(ctl_t *online_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    ol_t *ol = get_ol();
    if(0 == ol->count)     //无在线用户
    {
        ser_msg.ctl.cmd = 0;
        strcpy(ser_msg.reply, "没有在线用户!");
        send_msg(online_msg->cfd, &ser_msg, sizeof(ser_msg_t));
    }
    else
    {
        online_t *p = ol->ol_head;
        while(p->next)
        {
            online_t *tmp = p->next;
            p = p->next;
  
            ser_msg.ctl.cmd = 1;
            sprintf(ser_msg.reply, "%s\t\t%s", tmp->id, tmp->nickname);
            send_msg(online_msg->cfd, &ser_msg, sizeof(ser_msg_t));
            usleep(100000);
        }
        ser_msg.ctl.cmd = 0;
        sprintf(ser_msg.reply, "共%d位用户在线", ol->count);
        send_msg(online_msg->cfd, &ser_msg, sizeof(ser_msg_t));
    }
}

/****************************管理员处理函数***********************/
//管理员查看所有用户
static void check_all_handler(msg_sql_t *admin_msg)
{
    printf("%s\n",admin_msg->sql_buf);
    char **result;
    int row, col;
    ser_msg_t ser_msg;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    if(SQL_FAILURE == mysqlite_get_table(admin_msg->sql_buf, &result, &row, &col))
    {
        ser_msg.ctl.cmd = -1;
        strcpy(ser_msg.reply, "查看失败!");   
        send_msg(admin_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
    }
    else
    {
        if(0 == row)
        {
            ser_msg.ctl.cmd = 0;
            strcpy(ser_msg.reply, "数据库为空!");
            send_msg(admin_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
        }
        else
        {
            int i;
            for(i = 1; i <= row; i++)
            {
                ser_msg.ctl.cmd = 1;
                sprintf(ser_msg.reply, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\\%s\\%s\t%s\t%s",result[i*col+0],result[i*col+1],result[i*col+2],result[i*col+3],result[i*col+4],result[i*col+5],result[i*col+6],result[i*col+7],result[i*col+8],result[i*col+9],result[i*col+10],result[i*col+11]);
                send_msg(admin_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));
                memset(&ser_msg.reply, 0, sizeof(ser_msg_t));
            }
            ser_msg.ctl.cmd = 0;
            sprintf(ser_msg.reply, "共有%d位用户!", row);
            send_msg(admin_msg->ctl.cfd, &ser_msg, sizeof(ser_msg_t));    
        }
    }
}
//禁言与解除禁言
static void talk_handler(msg_who_t *ban_msg)
{
    ser_msg_t ser_msg;
    ser_msg.ctl.msg_size = sizeof(ser_msg_t);
    char *failure = NULL;
    char *success = NULL;
    if(BAN == ban_msg->ctl.cmd)
    {
        failure = "禁言失败!";
        success = "禁言成功!";
    }
    else
    {
        failure = "解除禁言失败!";
        success = "解除禁言成功!";
    }

    //修改在线ban
    online_t *p = ol_query_byid(ban_msg->who_id);
    if(NULL != p)
    {
        p->ban = (BAN == ban_msg->ctl.cmd) ? 1 : 0;
    }
    //修改数据库ban
    if(SQL_FAILURE == mysqlite_exec(ban_msg->sql_buf))
    {
        ser_msg.ctl.cmd = -1;
        strcpy(ser_msg.reply, failure);
        send_msg(ban_msg->ctl.cfd , &ser_msg, sizeof(ser_msg_t));
    }
    ser_msg.ctl.cmd = -1;
    strcpy(ser_msg.reply, success);
    send_msg(ban_msg->ctl.cfd , &ser_msg, sizeof(ser_msg_t));
}
//管理员替人下线
static void remove_user_handler(msg_who_t *rm_msg)
{
    online_t *p = ol_query_byid(rm_msg->who_id);
    ser_msg_t ser_msg;
    ser_msg.ctl.cmd = -1;
    if(NULL == p)
    {
        sprintf(ser_msg.reply, "%s不在线!", rm_msg->who_id);
        send_msg(rm_msg->ctl.cfd , &ser_msg, sizeof(ser_msg_t));
        return ;
    }
    //通知用户已被踢下线
    ser_msg.ctl.cmd = REMOVE_USER;
    strcpy(ser_msg.reply, "你已被管理员踢下线!");
    send_msg(p->cfd , &ser_msg, sizeof(ser_msg_t));

    //向管理员发送消息
    memset(&ser_msg, 0, sizeof(ser_msg_t));
    sprintf(ser_msg.reply, "%s已下线!", rm_msg->who_id);
    send_msg(rm_msg->ctl.cfd , &ser_msg, sizeof(ser_msg_t));
}

/*************************************************************************************/
//任务函数: 根据操作指令调用相应的处理函数
void *task_routine(void *arg)
{
    ctl_t ctl = *(ctl_t *)arg;
    switch (ctl.cmd)
    {
        //用户
        case REGISTER:              //注册
            reg_handler(arg);
            break;
        case LOGIN:                 //登录
            login_handler(arg);
            break;
        case FIND_PASSWORD:;        //找回密码
            find_psd_handler(arg);
            break;
        case PRIVATE_CHAT:          //私聊
        case GROUP_CAHT:            //群聊
            chat_handler(arg);
            break;
        case UPDATE:                //修改个人信息、开通VIP
            update_handler(arg);
            break;
        case CHECK_FILE:            //查看群文件
            check_file_handler(arg);
            break;
        case DOWNLOAD_FILE:         //下载文件
            download_file_handler(arg);
            break;
        case UPLOAD_FILE:           //上传文件
            upload_file_handler(arg);
            break;
        case LEAVE_WORD:            //留言
            leave_word_handler(arg);
            break;
        case LOOK_WORD:             //查看留言
            look_word_handler(arg);
            break;

        case CHECK_ONLINE:          //查看在线用户
            check_online_handler(arg);
            break;
        //管理员
        case CHECK_ALL:             //查看所有用户
            check_all_handler(arg);
            break;
        case BAN:                   //禁言
        case NOT_BAN:               //解除禁言
            talk_handler(arg);
            break;
        case REMOVE_USER:           //替人下线
            remove_user_handler(arg);
            break;
        default:
            break;
    } 
}
