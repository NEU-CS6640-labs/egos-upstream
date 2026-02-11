#pragma once

#include "servers.h"
#include <string.h>

enum syscall_type {
    SYS_RECV = 1,
    SYS_SEND = 2,
    /* [lab4-ex1]: a new sleeping syscall type */
    SYS_SLEEP = 3,
};

#define SYSCALL_MSG_LEN 1024
struct syscall {
    enum syscall_type type; /* SYS_SEND or SYS_RECV */
    int sender;             /* sender process ID    */
    int receiver;           /* receiver process ID  */
    union {
        char content[SYSCALL_MSG_LEN];
        uint value[SYSCALL_MSG_LEN/4];
    };
    enum { PENDING = 1, DONE = 2 } status;
};

void sys_send(int receiver, char* msg, uint size);
void sys_recv(int from, int* sender, char* buf, uint size);
/* [lab4-ex1]: wrapper function defined in syscall.c */
void sys_sleep(uint quantums);
