#ifndef __MYSQLITE_H__
#define __MYSQLITE_H__

#include <sqlite3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQL_FAILURE -1
#define SQL_SUCCESS 0

enum getdata_flags      //mysqlite_getdata flags
{
    LOGOUT,
    ONLINE,
    FIND    
};
//打开数据表
int mysqlite_open();
//数据库簇函数 -- 插入、删除、修改数据
int mysqlite_exec(const char *sql);
//关闭数据库
int mysqlite_close();
//获取单个用户的数据
int mysqlite_getdata(const char * sql, void *data, int flags);
//获取多个用户数据
int mysqlite_get_table(const char *sql, char ***result, int *row, int *col);

#endif