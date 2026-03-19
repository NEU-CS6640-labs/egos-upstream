#include "fs.h"
#include "string.h"
#include "stdlib.h"

extern fs_t fs; // defined in fs_rw.c
inode_t *__load_inode(int ino);

/* bitmap functions
 */
void bit_set(unsigned char *map, int i)
{
    map[i/8] |= (1 << (i%8));
}
void bit_clear(unsigned char *map, int i)
{
    map[i/8] &= ~(1 << (i%8));
}
int bit_test(unsigned char *map, int i)
{
    return map[i/8] & (1 << (i%8));
}

void block_read(uint block_no, block_t* block) {
    // invoking read() in sys_file.c
    (*fs.disk_read)(NULL, 0xffff, /* UNUSED */
                    (RWFS_DISK_START - FILE_SYS_DISK_START) + block_no,
                    block);
}

void block_write(uint block_no, block_t* block) {
    // invoking write() in sys_file.c
    (*fs.disk_write)(NULL, 0xffff, /* UNUSED */
                     (RWFS_DISK_START - FILE_SYS_DISK_START) + block_no,
                     block);
}


/* block allocate and free */
int alloc_block() {
    int ret;
    for (int i=0; i<fs.superblk.total_blks; i++) {
        if (!bit_test(fs.bitmap.bytes, i)) {
            bit_set(fs.bitmap.bytes, i);
            return i;
        }
    }
    FATAL("fs.c: alloc_block runs out of blocks");
    return -1;
}

void free_block(int blk_id) {
    bit_clear(fs.bitmap.bytes, blk_id);
    // clean block
    block_t buf;
    memset(&buf, 0, BLOCK_SIZE);
    block_write(blk_id, &buf);
}

/* inode translation from global to local */
uint ino_local2global(uint ino){
    ASSERT(ino < NINODES, "local inode is too small");
    return ino + fs.ino_offset;
}

uint ino_global2local(uint ino){
    ASSERT(ino >= fs.ino_offset, "global inode is too small");
    return ino - fs.ino_offset;
}

// ASSUMPTION: one data-block
static int lookup_name(inode_t *inode, char *token) {
    int inum;
    dirent_t dirent_arr[NUM_DIRENT_BLOCK];
    block_read(inode->ptrs[0], (block_t*)&dirent_arr);
    int name_exists = 0;
    for (int i = 0; i < NUM_DIRENT_BLOCK; i++) {
        if (dirent_arr[i].valid == 1 &&
            strcmp(dirent_arr[i].name, token) == 0) {
            inum = dirent_arr[i].inum;
            name_exists = 1;
            break;
        }
    }
    if (!name_exists) {
        return -1;
    }
    return inum;
}


int fs_dir_lookup(int g_dir_ino, const char *path) {
    char *_path = strdup(path);

    // global inode number to local number
    int inum = ino_global2local(g_dir_ino);
    int found = 0;
    char *token = strtok(_path, "/");
    while (token != NULL) {
        inode_t *inode = __load_inode(inum);
        if (inode->mode & MODE_D) {
            // inum is the next file
            inum = lookup_name(inode, token);
            if (inum == ROOT_PARENT_L_INO) {return ROOT_PARENT_G_INO;} /* return to parent fs */
            if (inum < 0) {return inum;}  /* hitting erros */
        } else { // should not see regular file
            INFO("ERROR: This is a regular file");
            free(_path);
            return -1;
        }

        token = strtok(NULL, "/");
    }
    free(_path);

    return ino_local2global(inum);
}
