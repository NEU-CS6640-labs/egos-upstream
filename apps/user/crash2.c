/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a program that attempts to perform privileged operations
 */

#include "app.h"
#include "egos.h"

int main() {
    asm("csrw mstatus, %0" ::"r"(0xdeadbeef));
    SUCCESS("Crash2 succeeds in running a high-privileged instruction");
}
