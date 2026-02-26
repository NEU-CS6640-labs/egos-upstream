/*
 * (C) 2025, Cornell University
 * All rights reserved.
 *
 * Description: a simple SD card driver
 *
 * Author: Yunhao Zhang
 * updated by OSI 2025 spring staff
 * updated by OSI 2026 spring staff
 */

#include "egos.h"
#include "string.h"

#define BLOCK_SIZE 512

/* See the "SD Host Controller Simplified Specification" (Part A2) document
   from the SD Association (https://www.sdcard.org/downloads/pls/) in which
   Chapter 2 "SD Host Standard Register" defines the register offsets below. */
#define SDHCI_DMA_ADDRESS        0x00
#define SDHCI_BLK_CNT_AND_SIZE   0x04
#define SDHCI_ARGUMENT           0x08
#define SDHCI_CMD_AND_MODE       0x0C
#define SDHCI_RESPONSE0          0x10
#define SDHCI_PRESENT_STATE      0x24
#define SDHCI_INT_STAT           0x30


// Bits in Command Register (offset 0x0E) (see S2.2.6)
#define DATA_PRESENT_FLAG         (1 << 5)

// Transfer Mode bits (offset 0x0C) (see S2.2.5)
#define TM_DMA_EN          (1u << 0)
#define TM_BLKCNT_EN       (1u << 1)
#define TM_AUTO_CMD12_EN   (1u << 2)
#define TM_READ            (1u << 4)
#define TM_WRITE           (0u << 4)
#define TM_MULTI_BLOCK     (1u << 5)


/* a buffer that is PAGE-aligned (so it is also BLOCK-aligned) */
static __attribute__((aligned(PAGE_SIZE))) char aligned_buf[PAGE_SIZE];

/*
 * Execute a single SD command through an SDHCI controller.
 *
 * Arguments:
 *   - cmd: SD command index (e.g., 17, 18, 24, 25).
 *   - arg: 32-bit command argument
 *   - flag: Command flags (response type, CRC/index check enable, DATA_PRESENT, etc.).
 *   - mode: Transfer Mode register value (DMA enable, read/write, multi-block, etc.).
 *
 * This function:
 *   1) Waits for the controller to become idle.
 *   2) Clears any pending interrupt state.
 *   3) Programs argument and issues the command atomically with transfer mode.
 *   4) Waits for Command Complete (or kernel panic on error).
 *   5) If the command has a data phase, waits for Transfer Complete.
 */
static void sdhci_exec_cmd(uint cmd, uint arg, uchar flag, uint mode) {
    /* 1. Wait until CMD and DAT are not inhibited. */
    while (REGW(SDHCI_BASE, SDHCI_PRESENT_STATE) & 0x3);

    /* 2. Clear all pending interrupt status bits. */
    REGW(SDHCI_BASE, SDHCI_INT_STAT) = 0xFFFFFFFF;

    /* 3. Write command argument and issue command. */
    REGW(SDHCI_BASE, SDHCI_ARGUMENT)     = arg;
    REGW(SDHCI_BASE, SDHCI_CMD_AND_MODE) =
        (((cmd << 8) | flag) << 16) | (mode & 0xFFFF);

    /* 4. Wait for Command Complete or FATAL when seeing errors. */
    while (1) {
        uint st = REGW(SDHCI_BASE, SDHCI_INT_STAT);   // polling

        // Error interrupt
        if (st & (1u << 15)) FATAL("SDHCI command hits errors");

        // Command Complete
        if (st & 0x1) break;
    }

    /* Clear Command Complete. */
    REGW(SDHCI_BASE, SDHCI_INT_STAT) = 0x1;

    /* 5. If this command includes a data phase, wait for Transfer Complete. */
    if (flag & DATA_PRESENT_FLAG) {
        while (1) {
            uint st = REGW(SDHCI_BASE, SDHCI_INT_STAT);  // polling

            /* Any data-phase error is fatal. */
            if (st & (1u << 15)) FATAL("SDHCI transfer error");

            /* SDMA boundary interrupt.
             * In full SDMA implementations, software must update
             * SDHCI_DMA_ADDRESS and continue. This implementation
             * assumes this won't happen, so kernel panic.
             */
            if (st & (1u << 3)) FATAL("Unexpected SDMA boundary interrupt");

            // Transfer Complete
            if (st & (1u << 1)) break;
        }

        /* Clear Transfer Complete. */
        REGW(SDHCI_BASE, SDHCI_INT_STAT) = (1u << 1);
    }
}


/* read one block from `block_no` and copy its contents to `dst` */
static void sdhci_single_read(uint block_no, char* dst) {
    /* Prepare DMA (SDMA mode of SDHCI). */
    REGW(SDHCI_BASE, SDHCI_DMA_ADDRESS)      = (uint)aligned_buf;
    REGW(SDHCI_BASE, SDHCI_BLK_CNT_AND_SIZE) = (1u << 16) | BLOCK_SIZE;



    /* [lab6-ex1]
     * Configure and issue a data command.
     *
     * - cmd:   command index (e.g., CMD17 for single-block read)
     * - lba:   logical block address corresponding to `block_no`.
     * - flag:  command configuration bits; must set the data-present bit
     *          to indicate an associated data phase. Response/error checks
     *          are omitted in this lab.
     * - mode:  transfer mode; (1) enable DMA and (2) specify read direction.
     */

    /* Send and wait for a read request with command #17. */
    FATAL("sdhci_single_read is not complete");

    uint  cmd  = 0;   /* TODO: set command index */
    uint  lba  = 0;   /* TODO: set block address for `block_no` */
    uchar flag = 0;   /* TODO: set command flags (data-present required) */
    uint  mode = 0;   /* TODO: set mode (see Transfer Mode bits, line 32) */
    /* TODO: call sdhci_exec_cmd() with proper arguments */

    /* copy contents to the dst memory */
    memcpy(dst, aligned_buf, BLOCK_SIZE);
}



/* write one block from `src` to block `block_no`
 *
 * [lab6-ex2]
 * Implement an SDHCI SDMA single-block write (CMD24).
 *
 * Steps:
 *  - prepare the DMA buffer (i.e., aligned_buf)
 *  - program SDHCI registers for the transfer:
 *    -- SDHCI_DMA_ADDRESS: set to the DMA buffer address.
 *    -- SDHCI_BLK_CNT_AND_SIZE: set both block count and block size
 *  - Issue CMD24: invoke `sdhci_exec_cmd()` with the correct argument
 *  - hint: refer to implementation of `sdhci_single_ead()`
 */
void sdhci_single_write(uint block_no, char *src) {
    /* TODO: your code here. */
    FATAL("sdhci_single_write is not implemented");

}


/* read multiple blocks from `block_no` and save them to `dst`
 *
 * [lab6-ex3]
 * Implement an SDHCI SDMA multi-block read (CMD18).
 *
 * Steps:
 *  - prepare the SDMA buffer; use a DMA-safe buffer (`aligned_buf`)
 *  - program SDHCI registers for this transfer:
 *      -- SDHCI_DMA_ADDRESS: set to the DMA buffer address (where DMA writes incoming data).
 *      -- SDHCI_BLK_CNT_AND_SIZE: program both the block count and block size
 *  - issue CMD18 (READ_MULTIPLE_BLOCK) with proper arguments
 *      -- `mode` must enable: multi-block, read direction, block count enable,
 *          DMA enable, and Auto CMD12 to terminate CMD18 cleanly
 *      -- call `sdhci_exec_cmd()` and wait for Transfer Complete.
 *  - copy data out
 */
static void sdhci_multi_read(uint block_no, int nblocks, char* dst) {
    ASSERT(nblocks <= (PAGE_SIZE / BLOCK_SIZE), "read too many blocks");

    /* TODO: your code here */
    FATAL("sdhci_multi_read() is not implemented");

}



/* write multiple blocks from `src` to the SD card, starting at `block_no`.
 *
 * [lab6-ex4]
 * Implement an SDHCI SDMA multi-block write (CMD25).
 *
 * Notes:
 *  - The structure matches `sdhci_multi_read()`: use a DMA-safe bounce buffer,
 *    program SDHCI_DMA_ADDRESS and SDHCI_BLK_CNT_AND_SIZE, then issue the command.
 *  - adjust the command index and transfer mode flags for a write
 */
static void sdhci_multi_write(uint block_no, int nblocks, char* src) {
    ASSERT(nblocks <= (PAGE_SIZE / BLOCK_SIZE), "write too many blocks");

    /* TODO: your code here */
    FATAL("sdhci_multi_write() is not implemented");

}

/* SD card read/write interfaces */
void sd_read(uint block_no, uint nblocks, char* dst) {
    ASSERT(nblocks > 0, "negative nblocks number");
    if (nblocks == 1) {
        sdhci_single_read(block_no, dst);
    } else {
        sdhci_multi_read(block_no, nblocks, dst);
    }
}

void sd_write(uint block_no, uint nblocks, char* src) {
    ASSERT(nblocks > 0, "negative nblocks number");
    if (nblocks == 1) {
        sdhci_single_write(block_no, src);
    } else {
        sdhci_multi_write(block_no, nblocks, src);
    }
}

void sd_test(); // test cases for lab6

/* Initialize the SD card during bootup */

void sd_init() {
#define PCI_ECAM_ALLOW_MMIO_AND_DMA ((1 << 1) | (1 << 2))
    /* Set the PCI ECAM base address register as SDHCI_BASE. */
    REGW(SDHCI_PCI_ECAM, 0x4)  = PCI_ECAM_ALLOW_MMIO_AND_DMA;
    REGW(SDHCI_PCI_ECAM, 0x10) = SDHCI_BASE;

    /* Reset the SD card and enable clock. */
#define SDHCI_CLKCON           0x2C
#define SDHCI_SOFTWARE_RESET   0x2F
    REGB(SDHCI_BASE, SDHCI_SOFTWARE_RESET) = 0x1;
    while (REGB(SDHCI_BASE, SDHCI_SOFTWARE_RESET) & 0x1);
    REGB(SDHCI_BASE, SDHCI_CLKCON) = 0x5;

    /* Enable interrupt status, but disable interrupt signal. */
#define SDHCI_INT_STAT_ENABLE  0x34
#define SDHCI_INT_SIG_ENABLE   0x38
    REGW(SDHCI_BASE, SDHCI_INT_SIG_ENABLE)  = 0x0;
    REGW(SDHCI_BASE, SDHCI_INT_STAT_ENABLE) = 0x27F003B;

    /* A simplified SDHCI initialization tailored for QEMU. */
    sdhci_exec_cmd(55, 0, 0, 0);
    sdhci_exec_cmd(41, 0xFFF0000, 0, 0);
    sdhci_exec_cmd(2, 0, 0, 0);
    sdhci_exec_cmd(3, 0, 2 /* get response */, 0);
    sdhci_exec_cmd(7, REGW(SDHCI_BASE, SDHCI_RESPONSE0), 0, 0);

    SUCCESS("Finished initializing SD card");

    /* [lab6-ex2]
     * Uncomment Lab6 test cases below to check your implementation.
     */
    // sd_test();
}



/* ============================
 * ==== SD Card Test Cases ====
 * ============================ */
#define SD_FREE_START (4*1024*1024) // empty start from 4MB

void test_write_single_block(int block_no, int * buf, char *buf2) {
    for (int i=0; i<BLOCK_SIZE/sizeof(int); i++) { // init buf
        buf[i] = i;
    }

    sd_write(block_no, 1, (char*)buf);  // write one block
    sd_read(block_no, 1, (char*)buf2);  // read the block back

    if (memcmp(buf, buf2, BLOCK_SIZE) == 0) { // check results
        SUCCESS("single-write test passed!");
    } else {
        FATAL("single-write test failed!");
    }
}

void test_read_multi_block(int block_no, int nblocks, int * buf, char *buf2) {
    for (int i=0; i<1024; i++) {     // init buf
        buf[i] = i*i;
    }
    for (int i=0; i<nblocks; i++) {
        sd_write(block_no+i, 1, &((char*)buf)[i*BLOCK_SIZE]); // write buf to SD card
    }
    sd_read(block_no, nblocks, buf2);
    if (memcmp(buf, buf2, 4096) == 0) {
        SUCCESS("multi-read test passed!");
    } else {
        FATAL("multi-read test failed!");
    }
}

void test_write_multi_block(int block_no, int nblocks, int * buf, char *buf2) {
    for (int i=0; i<1024; i++) { // init buf
        buf[i] = i*i*i;
    }
    sd_write(block_no, nblocks, (char*)buf); // write buf to SD card
    sd_read(block_no, nblocks, buf2);
    if (memcmp(buf, buf2, 4096) == 0) {
        SUCCESS("multi-write test passed!");
    } else {
        FATAL("multi-write test failed!");
    }
}

void sd_test() {
    CRITICAL("=== Start SD card testing ===");
    int buf[1024];    // 4KB
    char buf2[4096];  // 4KB
    uint block_no = SD_FREE_START / BLOCK_SIZE;

    // test 1: test write single block
    test_write_single_block(block_no, buf, buf2);
    // duplicated test for detecting residual states
    test_write_single_block(block_no, buf, buf2);

    // test 2: test multi-block read
    uint nblocks = 4096/BLOCK_SIZE;  // 4KB/512B -> 8
    test_read_multi_block(block_no, nblocks, buf, buf2);
    test_read_multi_block(block_no, nblocks, buf, buf2);

    // test 3: test multi-block write
    test_write_multi_block(block_no, nblocks, buf, buf2);
    test_write_multi_block(block_no, nblocks, buf, buf2);
    CRITICAL("=============================");
}
