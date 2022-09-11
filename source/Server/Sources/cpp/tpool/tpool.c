#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "tpool.h"

static tpool_t *tpool = NULL;

/* 工作者线程函数，从任务队列中取出任务并执行*/
static void *thread_routine(void *arg)
{
    tpool_task_t *task;

    while(1)
    {
        /* 如果任务队列为空，且线程池未关闭，线程阻塞等待任务*/
        pthread_mutex_lock(&(tpool->queue_lock));
        while(NULL == tpool->queue_head && 0 == tpool->shutdown)
        {
            pthread_cond_wait(&(tpool->queue_ready), &(tpool->queue_lock));
        }
        /* 查看线程池是否关闭，如果关闭，线程退出*/
        if(1 == tpool->shutdown)
        {
            pthread_mutex_unlock(&(tpool->queue_lock));
            pthread_exit(NULL);
        }
        /* 从任务队列中取出任务并执行*/
        task = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        task->next = NULL;
        pthread_mutex_unlock(&(tpool->queue_lock));
        
        task->routine(task->arg);
        /* 线程完成任务后释放任务*/
        free(task->arg);
        free(task);
        task = NULL;
    }
}

/* 创建线程池*/
int tpool_create(int max_thr_num)
{
    int i;

    /* 创建线程池结构体*/
    tpool = (tpool_t *)malloc(sizeof(tpool_t));
    if(NULL == tpool)
    {
        printf("%s: malloc tpool error\n", __FUNCTION__);
        exit(-1);
    }

    /* 初始化任务队列、互斥量、条件变量*/
    tpool->max_thr_num = max_thr_num;
    tpool->shutdown = 0;        //0代表线程池打开
    tpool->queue_head = NULL;
    tpool->queue_tail = NULL;
    if(pthread_mutex_init(&(tpool->queue_lock), NULL) != 0)
    {
        printf("%s: pthread_mutex_init error, errno: %d, error: %s\n", __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }
    if(pthread_cond_init(&(tpool->queue_ready), NULL) != 0)
    {
        printf("%s: pthread_cond_init error, errno: %d, error: %s\n", __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }

    /* 创建工作者线程*/
    tpool->thr_id = (pthread_t *)calloc(max_thr_num, sizeof(pthread_t));
    if(!tpool->thr_id)
    {
        printf("%s: calloc thr_id error\n", __FUNCTION__);
        exit(-1);
    }
    for(i = 0; i < max_thr_num; i++)
    {
        if(pthread_create(&(tpool->thr_id[i]), NULL, thread_routine, NULL) != 0)
        {
            printf("%s: pthread_create error, errno: %d, error: %s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        }
    }
    return 0;
}

/* 销毁线程池*/
void tpool_destroy()
{
    int i;
    tpool_task_t *member;
    if(tpool->shutdown)
    {
        return ;
    }
    /* 关闭线程开关*/
    tpool->shutdown = 1;
    /* 唤醒所有阻塞线程*/
    pthread_mutex_lock(&(tpool->queue_lock));
    pthread_cond_broadcast(&(tpool->queue_ready));
    pthread_mutex_unlock(&(tpool->queue_lock));

    /* 回收结束线程的资源*/
    for(i = 0; i < tpool->max_thr_num; i++)
    {
        pthread_join(tpool->thr_id[i], NULL);
    }

    /* 释放线程ID数组*/
    free(tpool->thr_id);
    /* 释放未完成的任务*/
    while(tpool->queue_head)
    {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        free(member->arg);
        free(member);
    }

    /* 销毁互斥锁和条件变量*/
    pthread_mutex_destroy(&(tpool->queue_lock));
    pthread_cond_destroy(&(tpool->queue_ready));

    /* 释放线程池结构体*/
    free(tpool);
}

/* 向线程池中添加任务*/
int tpool_add_task(void *(*routine)(void *), void *arg)
{
    /* work指向等待加入的任务队列的任务*/
    if(NULL == routine || NULL == arg)
    {
        printf("%s: Invalid argment\n", __FUNCTION__);
        return -1;
    }
    tpool_task_t *task = (tpool_task_t *)malloc(sizeof(tpool_task_t));
    if(NULL == task)
    {
        printf("%s: malloc argment\n", __FUNCTION__);
        return -1;
    }
    task->routine = routine;
    task->arg = arg;
    task->next = NULL;
	
    /* 将任务节点添加到任务队列中*/
    pthread_mutex_lock(&(tpool->queue_lock));
    /* 任务队列为空*/
    if(NULL == tpool->queue_head)
    {
        tpool->queue_head = task;
        tpool->queue_tail = task;
    }
    else
    {
        tpool->queue_tail->next = task;
        tpool->queue_tail = task;
    }
    /* 通知工作者线程，有新任务加入*/
    pthread_cond_signal(&(tpool->queue_ready));
    pthread_mutex_unlock(&(tpool->queue_lock));

    return 0;
}
