/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: helper functions for process management
 */

#include "process.h"

/* process status management */
static void proc_set_status(int pid, enum proc_status status) {
    for (uint i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == pid) proc_set[i].status = status;
}

void proc_set_ready(int pid) {
    proc_set_status(pid, PROC_READY);
}

void proc_set_running(int pid) {
    proc_set_status(pid, PROC_RUNNING);
}

void proc_set_runnable(int pid) {
    proc_set_status(pid, PROC_RUNNABLE);
}

void proc_set_pending(int pid) {
    proc_set_status(pid, PROC_PENDING_SYSCALL);
}


/* helper functions */
int pid2idx(int pid) {
    ASSERT(pid > 0 && pid <= MAX_NPROCESS, "wrong pid");
    return pid;
}

int idx2pid(int proc_idx) {
    ASSERT(proc_idx > 0 && proc_idx <= MAX_NPROCESS, "wrong proc_idx");
    return proc_idx;
}


/* process management */
int proc_alloc() {
    for (uint i = 1; i <= MAX_NPROCESS; i++) {
        if (proc_set[i].status == PROC_UNUSED) {
            proc_set[i].pid    = i;
            proc_set[i].status = PROC_LOADING;
            proc_on_arrive(proc_set[i].pid); // notify scheduler
            return proc_set[i].pid;
        }
    }

    FATAL("proc_alloc: reach the limit of %d processes", MAX_NPROCESS);
}

void proc_free(int pid) {
    if (pid != GPID_ALL) {
        earth->mmu_free(pid);
        proc_set_status(pid, PROC_UNUSED);
        proc_on_stop(pid); // notify scheduler
    } else {
        /* Free all user processes. */
        for (uint i = 0; i < MAX_NPROCESS; i++)
            if (proc_set[i].pid >= GPID_USER_START &&
                proc_set[i].status != PROC_UNUSED) {
                earth->mmu_free(proc_set[i].pid);
                proc_set[i].status = PROC_UNUSED;
                if (pid >= 0)
                    proc_on_stop(proc_set[i].pid); // notify scheduler
            }
    }
}

void proc_sleep(int pid, uint usec) {
    proc_set_status(pid, PROC_SLEEPING);
    proc_on_sleep(pid, usec); // notify scheduler
}

void proc_coresinfo() {
    /* Student's code goes here (Multicore & Locks). */

    /* Print out the pid of the process running on each CPU core. */

    /* Student's code ends here. */
}
