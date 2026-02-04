#include "process.h"
#include <string.h>

/* scheduling algorithms */
int round_robin();
/* defined in mlfq.c */
int mlfq();
void mlfq_update(int pid, enum state_transition tran, int time_units);

/* decide the next runnable process */
int schedule() {
    int next_idx = -1;
#ifndef MLFQ // no MLFQ
    next_idx = round_robin();
#else
    next_idx = mlfq();
#endif
    return next_idx;
}

int round_robin() {
    int next_idx = MAX_NPROCESS;
    for (uint i = 1; i <= MAX_NPROCESS; i++) {
        struct process* p = &proc_set[(curr_proc_idx + i) % MAX_NPROCESS];
        if (p->status == PROC_READY || p->status == PROC_RUNNABLE) {
            next_idx = (curr_proc_idx + i) % MAX_NPROCESS;
            break;
        }
    }
    return next_idx;
}

static void clear_pending_syscalls() {
    int progress = 0;
    do {
        progress = 0;
        for (uint i = 1; i <= MAX_NPROCESS; i++) {
            struct process* p = &proc_set[(curr_proc_idx + i) % MAX_NPROCESS];
            if (p->status == PROC_PENDING_SYSCALL) {
                proc_try_syscall(p);
                if (p->syscall.status == DONE) { // making progress
                    progress++;
                }
            }
        }
    } while(progress); // stop when making no progress
}

void proc_yield() {
    if (curr_status == PROC_RUNNING) {
        proc_set_runnable(curr_pid);
    }
    proc_on_sched_out(curr_pid);

    clear_pending_syscalls();

    // call scheduling algorithm to get the next runnable process
    int next_idx = schedule();
    ASSERT(next_idx < MAX_NPROCESS, "proc_yield: no process to run");

    curr_proc_idx = next_idx;
    earth->mmu_switch(curr_pid);
    earth->mmu_flush_cache();
    if (curr_status == PROC_READY) {
        /* Setup argc, argv and program counter for a newly created process. */
        curr_saved[0]                = APPS_ARG;
        curr_saved[1]                = APPS_ARG + 4;
        proc_set[curr_proc_idx].mepc = APPS_ENTRY;
    }
    proc_set_running(curr_pid);
    earth->timer_reset(core_in_kernel);
    proc_on_sched_in(curr_pid); // when switching process in to run
}


/* [lab3-ex2]
 * TODO: plan the layout and usage of `schd_attr`.
 *
 *   - You should design and document your own layout.
 *   - The macros below are examples only.
 *   - With such macros, attributes can be accessed as, for example:
 *
 *       proc_set[pid2idx(pid)].arrive_time = earth->gettime();
 *
 * Example mappings:
 *   #define arrive_time   schd_attr.longlongs[0]
 *   #define sched_count  schd_attr.ints[15]
 */




/* [lab3-ex2]
 * TODO:
 * In the following process state–transition callback functions,
 * update `sched_attr` in `struct process` (process.h) to record
 * the information required to compute scheduling metrics.
 *
 * Hints:
 *   - you should locate where each callback is invoked and understand the
 *     exact process state transition it represents.
 *   - Use `earth->gettime()` to obtain the current time.
 *     The returned value has type `unsigned long long` (`ulonglong`).
 *   - remember to initialize/clear `schd_attr` when the process starts to run
 *     for the first time.
 *   - Notice that pid=1 is different from others.
 *     When pid=1 arrives (see grass/init.c and process.c:proc_alloc()),
 *     it will be directly scheduled by grass layer, not via scheduler.c:schedule().
 * */

/* --------------------------------------
 * state-transition callback functions
 * --------------------------------------
 * */

void proc_on_arrive(int pid) {
    /* TODO: your code here */



#ifdef MLFQ
    mlfq_update(pid, PROC_ON_ARRIVE, 0);
#endif
}


void proc_on_sched_in(int pid) {
    /* TODO: your code here */



#ifdef MLFQ
    mlfq_update(pid, PROC_ON_SCHED_IN, 0);
#endif
}

void proc_on_sched_out(int pid) {
    /* TODO: your code here */



#ifdef MLFQ
    mlfq_update(pid, PROC_ON_SCHED_OUT, 0);
#endif
}

void proc_on_sleep(int pid, int time_units) {
    /* Callback invoked when a process transitions to sleep */
}

static float tar_time(int pid);
static float resp_time(int pid);
static int yield_num(int pid);
static float cpu_runtime(int pid);

void proc_on_stop(int pid) {
    /* TODO: your code here */



#ifdef MLFQ
    mlfq_update(pid, PROC_ON_STOP, 0);
#endif

    INFO("proc[%d] finished after %d yields, turnaround time: %f, response time: %f, cputime: %f",
            pid,
            yield_num(pid),
            tar_time(pid),
            resp_time(pid),
            cpu_runtime(pid));
}


/* --------------------------
 * process scheduling metrics
 * --------------------------
 * */

/* [lab3-ex2]
 * return the following scheduling metrics for `pid`.
 */

static float tar_time(int pid) {
    /* TODO: return the turnaround time for `pid`.
     * The return value is a floating-point number, expressed in units of QUANTUM.
     */

    return 0;
}

static float resp_time(int pid) {
    /* TODO: return the response time for `pid`.
     * The return value is a floating-point number, expressed in units of QUANTUM.
     */

  return 0;
}

static int yield_num(int pid) {
    /* TODO: return the number of times process `pid` has been scheduled,
     * i.e., the number of transitions into the RUNNING state.
     * The return value is an integer.
     */

    return 0;
}

static float cpu_runtime(int pid) {
    /* TODO: return the actual CPU time used by process `pid`.
     * The return value is a floating-point number, expressed in units of QUANTUM.
     */

    return 0;
}
