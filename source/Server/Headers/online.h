#ifndef __ONLINE_H__
#define __ONLINE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OL_SUCCESS 0
#define OL_FAILURE -1

typedef struct online
{
    char id[11];        //客户端用户ID
    char nickname[33];  //昵称
    int cfd;            //在线客户端的Socket文件描述符
    int ban;            //是否被禁言
    int vip;            //是否为VIP
    struct online *next;
}online_t;              //单个用户
typedef struct online_linklist
{
    int count;              //记录在线队列的用户数量
    online_t *ol_head;       //在线用户链表的头指针
}ol_t;                  //在线队列
//创建在线用户链表
int ol_create();
//头插法插入一个在线用户
int ol_insert_head(int cfd, const char *user_id, const char *nickname, int ban, int vip);
//通过用户Id查询在线用户
online_t *ol_query_byid(char *user_id);
//通过客户端cfd查询在线用户
online_t *ol_query_bycfd(int cfd);
//通过cfd删除一个在线用户
int ol_delete_bycfd(int cfd);
//返回在线链表头指针
ol_t *get_ol();
//销毁在线用户链表
void ol_destroy();

#endif