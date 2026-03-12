/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: the file system server
 * Manage the disk device; Handle file (inode) read and write for other apps.
 */

#include "app.h"
#include "inode.h"
#include "fs.h"

int getsize(inode_intf bs, uint ino) { return FILE_SYS_DISK_SIZE / BLOCK_SIZE; }

int setsize(inode_intf bs, uint ino, uint newsize) { FATAL("cannot set size"); }

int read(inode_intf bs, uint ino, uint offset, block_t* block) {
    earth->disk_read(FILE_SYS_DISK_START + offset, 1, block->bytes);
    return 0;
}

int write(inode_intf bs, uint ino, uint offset, block_t* block) {
    earth->disk_write(FILE_SYS_DISK_START + offset, 1, block->bytes);
    return 0;
}

int main() {
    SUCCESS("Enter kernel process GPID_FILE");

    /* Initialize the file system interface. */
    struct inode_store disk = (struct inode_store){
        .read = read,
        .write = write,
        .getsize = getsize,
        .setsize = setsize };

    inode_intf fs =
        (FILESYS == 0) ? mydisk_init(&disk, 0) : treedisk_init(&disk, 0);

#ifdef RWFSON
    fs_init(&disk, NINODES);
#endif

    /* Send a notification to GPID_PROCESS. */
    char buf[SYSCALL_MSG_LEN];
    strcpy(buf, "Finish GPID_FILE initialization");
    grass->sys_send(GPID_PROCESS, buf, 32);

    /* Wait for inode read or write requests. */
    while (1) {
        int sender, r;
        struct file_request* req = (void*)buf;
        struct file_reply* reply = (void*)buf;
        grass->sys_recv(GPID_ALL, &sender, buf, SYSCALL_MSG_LEN);

        switch (req->type) {
        case FILE_READ:
            if (req->ino < NINODES) { // use default RO fs
                r = fs->read(fs, req->ino, req->offset, (void*)&reply->block);
            } else {
#ifdef RWFSON
                r = fs_read(req->ino, req->offset, req->len, reply->block.bytes);
#else
                FATAL("inode number is too large");
#endif
            }
            reply->status = r == 0 ? FILE_OK : FILE_ERROR;
            grass->sys_send(sender, (void*)reply, sizeof(*reply));
            break;

        case FILE_WRITE:
#ifdef RWFSON
            ASSERT(req->ino >= NINODES, "Updating RO fs");

            r = fs_write(req->ino, req->offset, req->len, req->block.bytes);
            reply->status = r == 0 ? FILE_OK : FILE_ERROR;
            grass->sys_send(sender, (void*)reply, sizeof(*reply));
            break;
#else
            FATAL("RO fs does not support FILE_WRIE");
#endif

        case FILE_GETSIZE:
#ifdef RWFSON
            ASSERT(req->ino >= NINODES, "RO fs does not support FILE_GETSIZE");

            int fsz = fs_getsize(req->ino);
            reply->status = (fsz >= 0) ? FILE_OK : FILE_ERROR;
            *(uint*)reply->block.bytes = fsz; // store the size in the first 4B
            grass->sys_send(sender, (void*)reply, sizeof(*reply));
            break;
#else
            FATAL("RO fs does not support FILE_GETSIZE");
#endif

        case DIR_LOOKUP:
#ifdef RWFSON
            ASSERT(req->ino >= NINODES, "RO fs does not support DIR_LOOKUP");

            char *path = req->block.bytes;
            int ino = fs_dir_lookup(req->ino, path);
            reply->status = (ino >= 0) ? FILE_OK : FILE_ERROR;
            *(uint*)reply->block.bytes = ino; // store the inode number in the first 4B
            grass->sys_send(sender, (void*)reply, sizeof(*reply));
            break;
#else
            FATAL("RO fs does not support DIR_LOOKUP");
#endif

        default:
            FATAL("sys_file: invalid request %d", req->type);
        }
    }
}
