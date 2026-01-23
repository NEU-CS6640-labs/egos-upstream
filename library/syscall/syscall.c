/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: the system call interface for applications
 */

#include "egos.h"
#include "syscall.h"

static struct syscall* sc = (struct syscall*)SYSCALL_ARG;

static inline void trap() {
    uint hartid;
    asm("csrr %0, mhartid" : "=r"(hartid));
    *(volatile uint *)CLINT_MSIP(hartid) = 1;

    while(sc->status == PENDING); // loop until the syscall is handled
}

void sys_send(int receiver, char* msg, uint size) {
    sc->type     = SYS_SEND;
    sc->receiver = receiver;
    sc->status = PENDING;
    memcpy(sc->content, msg, size);
    trap();
}

void sys_recv(int from, int* sender, char* buf, uint size) {
    sc->type   = SYS_RECV;
    sc->sender = from;
    sc->status = PENDING;
    trap();
    memcpy(buf, sc->content, size);
    if (sender) *sender = sc->sender;
}
