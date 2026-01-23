/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a simple TTY device driver (QEMU virt UART only)
 */

#include "egos.h"

#define VIRT_LINE_STATUS 5UL

uint uart_rx_empty() {
    return !(REGB(UART_BASE, VIRT_LINE_STATUS) & (1 << 0));
}

void uart_getc(char* c) {
    while (uart_rx_empty());
    *c = REGW(UART_BASE, 0) & 0xFF;
}

void uart_putc(char c) {
    while (!(REGB(UART_BASE, VIRT_LINE_STATUS) & (1 << 5)));
    REGW(UART_BASE, 0) = c;
}

void tty_init() {
    earth->tty_read        = uart_getc;
    earth->tty_write       = uart_putc;
    earth->tty_input_empty = uart_rx_empty;
}
