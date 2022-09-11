#ifndef __SERVER_FUNC_H__
#define __SERVER_FUNC_H__

#include "internet.h"
#include "myepoll.h"
#include "tpool.h"
#include "mysqlite.h"
#include "online.h"
#include "tcp_server.h"
#include "myfile.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//任务函数
void *task_routine(void *arg);

void id_init(void);

void id_destroy(void);

#endif