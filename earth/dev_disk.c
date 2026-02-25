/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a simple disk device driver
 */

#include "egos.h"
#include "disk.h"
#include <string.h>

/* defined in dev_sdcard.c */
int sd_init();
void sd_read(int block_no, int nblocks, char* dst);
void sd_write(int block_no, int nblocks, char* src);

static enum disk_type { SD_CARD, FLASH_ROM } type;

void disk_read(uint block_no, uint nblocks, char* dst) {
    if (type == FLASH_ROM) {
        char* src = (char*)FLASH_ROM_BASE + block_no * BLOCK_SIZE;
        memcpy(dst, src, nblocks * BLOCK_SIZE);
        return;
    }  else if (type == SD_CARD) {
        sd_read(block_no, nblocks, dst);
    }  else {
        FATAL("UNKNOWN disk type.");
    }
}

void disk_write(uint block_no, uint nblocks, char* src) {
    if (type == FLASH_ROM) {
        FATAL("FLASH_ROM is read only");
    } else if (type == SD_CARD) {
        sd_write(block_no, nblocks, src);
    } else {
        FATAL("UNKNOWN disk type.");
    }
}

void disk_init() {
#ifndef SDCARD
    type = FLASH_ROM;
#else
    type = SD_CARD;
    sd_init();
#endif

    earth->disk_read  = disk_read;
    earth->disk_write = disk_write;
}
