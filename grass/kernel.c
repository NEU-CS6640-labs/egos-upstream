/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: kernel ≈ 3 handlers
 *   interrupt handler
 *   exception handler
 *   syscall
 */

#include "process.h"
#include <string.h>

uint core_in_kernel;
uint core_to_proc_idx[NCORES];
struct process proc_set[MAX_NPROCESS + 1];
/* proc_set[0] is a place holder for idle cores. */

static void intr_entry(uint);
static void excp_entry(uint);

void kernel_entry() {
    /* With the kernel lock, only one core can enter this point at any time. */
    asm("csrr %0, mhartid" : "=r"(core_in_kernel));

    /* Save the process context. */
    asm("csrr %0, mepc" : "=r"(proc_set[curr_proc_idx].mepc));
    memcpy(curr_saved, (void*)(EGOS_STACK_TOP - 32 * 4), 32 * 4);

    uint mcause;
    asm("csrr %0, mcause" : "=r"(mcause));
    if (mcause & (1 << 31)) {
        intr_entry(mcause & 0x3FF);
    } else {
        excp_entry(mcause);
    }

    /* Restore the process context. */
    asm("csrw mepc, %0" ::"r"(proc_set[curr_proc_idx].mepc));
    memcpy((void*)(EGOS_STACK_TOP - 32 * 4), curr_saved, 32 * 4);

    /* [lab4-ex4]
     * when resuming an app, the kernel switches privilege level:
     * it updates mstatus.MPP to select the target mode:
     * - if curr_pid is a user application, switch to U-mode
     * - if curr_pid is a system process, switch to M-mode
     */

    /* TODO: your code here */

} // will return to grass/kernel.s, which finally calls `mret`

#define INTR_ID_SOFT_M  3
#define INTR_ID_TIMER   7
#define EXCP_ID_ECALL_U 8
#define EXCP_ID_ECALL_M 11

static void excp_entry(uint id) {
    /* [lab4-ex3]
     * - If id is for syscalls, handle the system call and return:
     *   -- you need to capture **all** ecall exceptions
     *   -- you need to think of which pc the CPU will run after "mret";
     *      in other word, you need to properly update PCBs so that
     *      eventually "mepc" will be set to the right instruction.
     *   -- How to invoke syscall? check out our old implementation using
     *      software interrupt (in intr_entry() below)
     * - Otherwise,
     *   -- if curr_pid is a user application, kill the process
     *   -- if curr_pid is a system proc, panic the kernel using FATAL
     */

    /* TODO: your code here */
    FATAL("excp_entry: kernel got exception %d", id);
}

static void intr_entry(uint id) {
    if (id == INTR_ID_TIMER) {
        /* user process killed by ctrl+c */
        char c;
        if (curr_pid >= GPID_USER_START &&
            !earth->tty_input_empty() &&
            (earth->tty_read(&c), c == 0x03) )
        {
            INFO("process %d killed by interrupt", curr_pid);
            proc_set[curr_proc_idx].mepc = (uint) (APPS_ENTRY + 0xC);
            return;
        }
        proc_yield();
#ifndef ECALL
    } else if (id == INTR_ID_SOFT_M) {
        /* clear the soft interrupt */
        uint hartid;
        asm("csrr %0, mhartid" : "=r"(hartid));
        *(volatile uint *)CLINT_MSIP(hartid) = 0;

        /* Copy the system call arguments from user space to the kernel. */
        uint syscall_paddr = earth->mmu_translate(curr_pid, SYSCALL_ARG);
        memcpy(&proc_set[curr_proc_idx].syscall,
               (void*)syscall_paddr,
               sizeof(struct syscall));
        proc_set[curr_proc_idx].syscall.status = PENDING;

        proc_set_pending(curr_pid);
        proc_try_syscall(&proc_set[curr_proc_idx]);
        proc_yield();
        return;
#endif
    } else {
        FATAL("intr_entry: kernel got interrupt %d", id);
    }
}

