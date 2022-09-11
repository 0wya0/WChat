#include "myfile.h"

//打开文件
FILE *myfopen(const char *file, const char *mode)
{
    FILE *fp = fopen(file, mode);
    if(NULL == fp)
    {
        perror("myfopen error");
        return NULL;
    }
    return fp;
}
//写文件
int myfwrite(FILE *fp, const void *buf, int size)
{
    if(-1 == fwrite(buf, 1, size, fp))
    {
        perror("mywrite error");
        return MF_FAILURE;
    }
    return MF_SUCCESS;
}
//读文件
int myfread(FILE *fp, char *buf, int size)
{
    if(-1 == fread(buf, 1, size, fp))
    {
        perror("myread error");
        return MF_FAILURE;
    }
    return MF_SUCCESS;
}
//关闭文件
int myfclose(FILE *fp)
{
    if(-1 == fclose(fp))
    {
        perror("myclose error");
        return MF_FAILURE;
    }
    return MF_SUCCESS;
}
