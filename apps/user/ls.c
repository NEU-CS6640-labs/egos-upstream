/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a simple ls
 * Updated by OSI staff 23fall and 26spring
 */

#include "app.h"
#include "string.h"
#include "fs.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        INFO("ls: ls with args is not implemented");
        return -1;
    }

    /* Read the directory content. */
    char buf[BLOCK_SIZE];
    file_read(workdir_ino, 0, BLOCK_SIZE, buf);

    if (workdir_ino < NINODES) {
        /* this is RO fs */
        /* Remove the inode numbers from the string. */
        for (uint i = 1; i < strlen(buf); i++)
            if (buf[i - 1] == ' ' && buf[i] >= '0' && buf[i] <= '9') buf[i] = ' ';
        /* Print out the directory content. */
        printf("%s\n\r", buf);
    } else {
#ifdef RWFSON
        /* this is rwfs */
        dirent_t* entries = (dirent_t*) buf;
        for (int i=0; i<NUM_DIRENT_BLOCK; i++) {
            if (entries[i].valid) {
                printf("%s   ", entries[i].name);
            }
        }
        printf("\n");
#else
        FATAL("rwfs is off; should never be here");
#endif
    }

    return 0;
}
