#ifndef __MYFILE_H__
#define __MYFILE_H__

#include <stdio.h>

#define MF_SUCCESS 0
#define MF_FAILURE -1

#include "myfile.h"

//打开文件
FILE *myfopen(const char *file, const char *mode);

//写文件
int myfwrite(FILE *fp, const void *buf, int size);

//读文件
int myfread(FILE *fp, char *buf, int size);

//关闭文件
int myfclose(FILE *fp);

#endif