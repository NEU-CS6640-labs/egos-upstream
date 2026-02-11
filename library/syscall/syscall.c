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
#ifndef ECALL
    uint hartid;
    asm("csrr %0, mhartid" : "=r"(hartid));
    *(volatile uint *)CLINT_MSIP(hartid) = 1;

    while(sc->status == PENDING); // loop until the syscall is handled
#else
    /* [lab4-ex2]
     * TODO: implement trap using ecall
     */
    FATAL("trap is not implemented");
#endif
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


/* [lab4-ex1]
 * Implement this wrapper function (to be invoked by user-level applications).
 * The function should trigger a trap into the kernel, marking this proc
 * as a sleeping process for `quantums` units of time.
 */
void sys_sleep(uint quantums) {
    /* TODO: your code here */

    FATAL("sys_sleep is not implemented");
}
