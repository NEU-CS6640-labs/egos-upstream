/*
 * Description: a UNIX-like file system
 *
 * Created by OSI staff 23fall, updated by OSI staff 26spring
 */

#include "fs.h"
#include "string.h"
#include "stdlib.h"

/* bit manipulation
 * see implementation in fs.c */
int bit_test(unsigned char *map, int i);

/* disk read/write
 * see implementation in fs.c */
void block_read(int block_no, block_t* dst);
void block_write(int block_no, block_t* src);

/* block allocate and free
 * see implementation in fs.c */
int alloc_block();
void free_block(int blk_id);

/* global fs_t instance */
fs_t fs;




// [lab7-ex1] initialize fs
// This function initialize the global fs variable "fs".
//  -- read the definition of "fs_t" and "super_t" in "fs.h"
//  -- you should read superblock from disk by "block_read"
//  -- you should update the "avail_blks" by counting empty blocks in bitmap
//     (bit_test is a helper function to count)
//  -- after your initialization, the assertions should pass
void fs_init(inode_intf disk, uint ino_offset) {
    fs.ino_offset = ino_offset;
    fs.disk_read = disk->read;
    fs.disk_write = disk->write;

    // TODO: your code here
    FATAL("fs_init is not implemented");

    // fs metadata checks
    ASSERT(fs.superblk.magic == 0x6640, "fs magic number mismatch");
    ASSERT(fs.superblk.total_blks == RWFS_DISK_SIZE/BLOCK_SIZE, "incorrect disk block size");
    ASSERT(fs.avail_blks == 4069, "incorrect available blocks"); // used for lab7 check only
}

// [lab7-ex2]
// This function reads inodes from disk.
// You should load a block from the inode arrary on disk
// which contains the inode "ino":
//   -- calculate which block contains inode "ino"
//      (note: "ino" is local to rwfs; meaning, "ino=0" represents the root of
//             rwfs, not the entire fs)
//   -- load the block to fs.buffer_cache
//      (note: inode array starts at block INODEARR_BLOCK_START)
//   -- return the inode pointer (pointing into fs.buffer_cache)
inode_t *__load_inode(int ino) {
    ASSERT(ino < NINODES, "ino should be local to rwfs");

    // TODO: your code here

    return NULL;
}


// [lab7-ex2]
// write the contents of "fs.buffer_cache" (a list of inodes) back to disk
// notes:
//   -- you need to remember the block id of the cached inodes
//   -- later when you update files, remember to flush inodes when things change
void flush_inode() {
    // TODO: your code here

}


// [lab7-ex2]
// read "len" bytes of contents starting from "offset"
// in the file whose inode number is "ino" (global inode number "g_ino").
// You should:
//   -- load the inode data structure
//   -- calculate which blocks (directed or indirected) contain the wanted data
//   -- copy the data to "buf"
//   -- return 0 if success; otherwise, return -1
// notes:
//   -- you can safely assume "buf" is the size of "len"
//   -- for simplicity, "len" will not exceed 1 block
int fs_read(int g_ino, int offset, int len, char *buf) {
    ASSERT(g_ino >= NINODES, "g_ino is not a global inode number");
    ASSERT(len > 0 && len <= BLOCK_SIZE, "len is invalid");

    uint ino = ino_global2local(g_ino);

    // TODO: your code here
    FATAL("fs_read is not implemented");

    return 0;
}


// [lab7-ex3]
// return the size of the file "g_ino"
// note:
// -- check "inode_t" in fs.h
// -- "g_ino" is the global inode number
int fs_getsize(uint g_ino) {
    ASSERT(g_ino >= NINODES, "g_ino is not a global inode number");

    // TODO: your code here
    FATAL("fs_getsize is not implemented");

    return 0;
}

// [lab7-ex3]
// write "len" bytes of data to the file ("g_ino") starting from "offset"
// notes:
//   -- "g_ino" is a global inode number
//   -- use "alloc_block" to allocate a block if needed
//   -- remember to update file size
//   -- remember to flush the inode
//   -- remember to flush the modified blocks
//   -- return 0 if success; otherwise, return -1
int fs_write(int g_ino, int offset, int len, const char *buf) {
    ASSERT(g_ino >= NINODES, "g_ino is not a global inode number");
    ASSERT(len > 0 && len <= BLOCK_SIZE, "len is invalid");

    // TODO: your code here
    FATAL("fs_write is not implemented");

    return 0;
}
