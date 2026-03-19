#pragma once
#include "egos.h"
#include "disk.h"
#include "inode.h"

/* a fake inode number pointing to the parent of rwfs mount point */
#define ROOT_PARENT_L_INO   6640
/* global inode number for the parent of rwfs (see mkfs.c) */
#define ROOT_PARENT_G_INO   2

/* how many buckets of size M do you need to hold N items? */
#define DIV_ROUND_UP(N, M) ((N) + (M) - 1) / (M)
/* number of direct pointers */
#define NUM_PTRS 10
/* number of indirect pointers */
#define NUM_INPTRS (BLOCK_SIZE / sizeof(uint))
/* file size */
#define FILE_SIZE (BLOCK_SIZE * (NUM_PTRS + NUM_INPTRS))
/* number of directory entries (dirent_t) in one block */
#define NUM_DIRENT_BLOCK (BLOCK_SIZE / sizeof(struct fs_dirent))

#define SUPER_BLOCK_ID       0
#define BITMAP_BLOCK_ID      1
#define INODEARR_BLOCK_START 2
#define DATA_BLOCK_START     12
#define DATA_BLOCK_END       4095

/* Superblock */
typedef struct fs_super {
    uint magic;       /*==0x6640*/
    uint total_blks;  /* number of blocks */

    /* pad out to an entire block */
    char pad[BLOCK_SIZE - 2 * sizeof(uint)];
} super_t;

typedef struct fs_inode {
    uint mode;
    uint size;     /* file size in bytes */
    uint pads[3];
    uint ptrs[NUM_PTRS]; /* direct data block pointer */
    uint indirect_ptr; /* indirect pointer */
} inode_t;

/* Entry in a directory */
typedef struct fs_dirent {
    uint valid : 1;
    uint inum  : 31;
    char name[28];         /* with trailing NUL */
} dirent_t;


/* in-memory fs data structure*/
typedef struct fs_struct {
    super_t superblk;
    uint avail_blks;

    // help translate between local inode numbers to global inode numbers
    uint ino_offset;
    // disk read/write functions
    int (*disk_read)(inode_intf bs, uint ino, uint offset, block_t* block);
    int (*disk_write)(inode_intf bs, uint ino, uint offset, block_t* block);

    block_t bitmap;

    // page caches
    uint buffer_blk_id;
    block_t buffer_cache;
} fs_t;

/*
 file or dir
 |<-   ->|                   |<- S-app ->|<- U-app ->|
 +---+---+---+---+---+---+---+---+---+---+---+---+---+
 | F | D |   |   |   |   |   | R | W | X | R | W | X |
 +---+---+---+---+---+---+---+---+---+---+---+---+---+
*/

#define MODE_F  (1 << 31)
#define MODE_D  (1 << 30)
#define MODE_UW (1 << 1)
#define MODE_UR (1 << 2)
#define MODE_SW (1 << 4)
#define MODE_SR (1 << 5)
#define MODE_UALL (MODE_UW | MODE_UR)
#define MODE_SALL (MODE_SW | MODE_SR)
#define MODE_ALL  (MODE_UALL | MODE_SALL)

/* translating inode numbers between global and local (rwfs) */
uint ino_local2global(uint ino);
uint ino_global2local(uint ino);

/* fs functions */
void fs_init(inode_intf disk, uint offset_ino);
int  fs_dir_lookup(int dir_ino, const char *path);

int  fs_read(int ino, int offset, int len, char *buf);
int  fs_write(int ino, int offset, int len, const char *buf);
int  fs_getsize(uint ino);
