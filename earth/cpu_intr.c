/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: wrapping the CPU interface for interrupts
 * Initialize the trap entry, enable interrupts, and reset the timer.
 *
 * Updated by CS6640 26spring staff.
 */

#include "egos.h"

void trap_entry(); /* See grass/kernel.s */

void intr_init(uint core_id) {
    /* Setup the interrupt/exception handling entry. */
    asm("csrw mtvec, %0" ::"r"(trap_entry));
    INFO("Use direct mode and put the address of the trap_entry into mtvec");

    /* Enable timer interrupt. */
    asm("csrw mip, %0" ::"r"(0));
    uint flags = 0x80 | /* MTIE */
                 0x8;   /* MSIE */
    asm("csrs mie, %0" ::"r"(flags));
    asm("csrs mstatus, %0" ::"r"(0x88));
}
