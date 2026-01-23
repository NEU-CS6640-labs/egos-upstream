#pragma once

#include "egos.h"
#include "syscall.h"

enum proc_status {
    PROC_UNUSED,
    PROC_LOADING,
    PROC_READY,
    PROC_RUNNING,
    PROC_RUNNABLE,
    PROC_PENDING_SYSCALL
};

struct process {
    int pid;
    struct syscall syscall;
    enum proc_status status;
    uint mepc, saved_registers[32];
};
#define MAX_NPROCESS 16

ulonglong mtime_get();

int proc_alloc();
void proc_free(int);
void proc_set_ready(int);
void proc_set_running(int);
void proc_set_runnable(int);
void proc_set_pending(int);

void mlfq_reset_level();
void mlfq_update_level(struct process* p, ulonglong runtime);
void proc_sleep(int pid, uint usec);
void proc_coresinfo();

extern uint core_to_proc_idx[NCORES];


/* manaing process status */
/* defined in kernel.c */
extern uint core_in_kernel;
extern uint core_to_proc_idx[NCORES];
extern struct process proc_set[MAX_NPROCESS + 1];

#define curr_proc_idx (core_to_proc_idx[core_in_kernel])
#define curr_pid      (proc_set[curr_proc_idx].pid)
#define curr_status   (proc_set[curr_proc_idx].status)
#define curr_saved    (proc_set[curr_proc_idx].saved_registers)


/* kernel functions */
void proc_yield();  /* defined in scheduler.c */
void proc_try_syscall(struct process* proc); /* defined in syscall.c */

