/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: user-friendly interface for system processes
 */

#include "egos.h"
#include "syscall.h"
#include <stdlib.h>
#include "fs.h"

static int sender;
static char buf[SYSCALL_MSG_LEN];

void exit(int status) {
    struct proc_request req;
    req.type = PROC_EXIT;
    sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
    while (1);
}

int dir_lookup(int dir_ino, char* name) {
    if (dir_ino < NINODES) { // RO fs
        char buf[BLOCK_SIZE];
        file_read(dir_ino, 0, BLOCK_SIZE, buf);

        /* Read tools/mkfs.c to understand directory management. */
        for (uint i = 0, namelen = strlen(name); i < strlen(buf) - namelen; i++) {
            if (!strncmp(name, buf + i, namelen) &&
                    buf[i + namelen] == ' ' &&
                    (i == 0 || buf[i - 1] == ' ')) {
                return atoi(buf + i + namelen);
            }
        }
    } else { // RW fs
#ifdef RWFSON
        struct file_request req;
        req.type   = DIR_LOOKUP;
        req.ino    = dir_ino;
        strcpy(req.block.bytes, name);

        sys_send(GPID_FILE, (void*)&req, sizeof(req));
        sys_recv(GPID_FILE, &sender, buf, SYSCALL_MSG_LEN);

        struct file_reply* reply = (void*)buf;
        int ino = *(uint*)reply->block.bytes; // store the inode number in the first 4B
        return ino;
#else
        FATAL("RWFS is off; should not be here");
#endif
    }

    return -1;
}

int file_read(int file_ino, uint offset, uint len, char* dst) {
    ASSERT(len <= BLOCK_SIZE, "len is greater than 1 block");

    struct file_request req;
    req.type   = FILE_READ;
    req.ino    = file_ino;
    req.offset = offset;
    req.len    = len;

    sys_send(GPID_FILE, (void*)&req, sizeof(req));
    sys_recv(GPID_FILE, &sender, buf, SYSCALL_MSG_LEN);

    struct file_reply* reply = (void*)buf;
    memcpy(dst, reply->block.bytes, len);

    return reply->status == FILE_OK ? 0 : -1;
}

int file_write(int file_ino, uint offset, uint len, char* src) {
    ASSERT(len <= BLOCK_SIZE, "len is greater than 1 block");

    struct file_request req;
    req.type   = FILE_WRITE;
    req.ino    = file_ino;
    req.offset = offset;
    req.len    = len;

    memcpy(req.block.bytes, src, len);

    sys_send(GPID_FILE, (void*)&req, sizeof(req));
    sys_recv(GPID_FILE, &sender, buf, SYSCALL_MSG_LEN);

    struct file_reply* reply = (void*)buf;
    return reply->status == FILE_OK ? 0 : -1;
}


int file_getsize(int file_ino, uint* fsz) {
    struct file_request req;
    req.type   = FILE_GETSIZE;
    req.ino    = file_ino;

    sys_send(GPID_FILE, (void*)&req, sizeof(req));
    sys_recv(GPID_FILE, &sender, buf, SYSCALL_MSG_LEN);

    struct file_reply* reply = (void*)buf;
    *fsz = *(uint*)reply->block.bytes; // store the size in the first 4B
    return reply->status == FILE_OK ? 0 : -1;
}

#ifndef KERNEL

/* Terminal read/write for user applications send messages to GPID_TERMINAL. */
int term_read(char* buf, uint len) {
    struct term_request req;
    struct term_reply reply;
    req.type = TERM_INPUT;
    req.len  = len;
    sys_send(GPID_TERMINAL, (void*)&req, sizeof(req));
    sys_recv(GPID_TERMINAL, NULL, (void*)&reply, sizeof(reply));
    memcpy(buf, reply.buf, reply.len);
    return reply.len;
}

void term_write(char* str, uint len) {
    struct term_request req;
    req.type = TERM_OUTPUT;
    req.len  = len;
    memcpy(req.buf, str, len);
    sys_send(GPID_TERMINAL, (void*)&req, sizeof(req));
}

#else

/* Terminal read/write for the kernel directly use the TTY earth interface. */
int term_read(char* buf, uint len) {
    char c;
    for (int i = 0; i < len - 1; i++) {
        earth->tty_read(&c);
        buf[i] = c;

        switch (c) {
        case 0x0d: /* Enter     */
            buf[i] = 0;
            term_write("\n\r", 2);
            return i ? i + 1 : 0;
        case 0x7f: /* Backspace */
            c = 0;
            if (i) term_write("\b \b", 3);
            i = i ? i - 2 : i - 1;
        }
        if (c) term_write(&c, 1);
    }

    buf[len - 1] = 0;
    return len;
}

void term_write(char* str, uint len) {
    for (uint i = 0; i < len; i++) earth->tty_write(str[i]);
}

#endif
