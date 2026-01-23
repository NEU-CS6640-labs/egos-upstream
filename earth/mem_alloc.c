/* Author: CS6640 23fall staff
 * Description: a simple memory allocator
 * Updated by CS6640 26spring staff
 */

#include "egos.h"
#include "string.h"

/* manage memory pages of (only app related pages):
 *    [APPS_PAGES_BASE, RAM_END)
 * */

#define ROUNDUP(x,y) (x%y==0? x/y : (x+y-1)/y)

/* a bit map to track page availability
 * 0 for free; 1 for used
 */
unsigned char bitmap[ROUNDUP(APPS_PAGES_CNT,8)];

/* bitmap functions
 */
void bitmap_set(uint i) {
    bitmap[i/8] |= (1 << (i%8));
}

void bitmap_clear(uint i) {
    bitmap[i/8] &= ~(1 << (i%8));
}

int bitmap_test(uint i) {
    return bitmap[i/8] & (1 << (i%8));
}

/*
 * locking for multi-core
 */
void PMEM_LOCK() {/*TODO*/}
void PMEM_UNLOCK() {/*TODO*/}


/* memory allocator interfaces
 */

void *pgid_to_paddr(uint pgid) {
    ASSERT(pgid < APPS_PAGES_CNT, "pgid_to_paddr: pgid is too large");
    return (void *) (APPS_PAGES_BASE + pgid * PAGE_SIZE);
}

uint paddr_to_pgid(void *paddr) {
    ASSERT( (uint)paddr % PAGE_SIZE == 0, "paddr_to_pgid: paddr is not page aligned");
    uint pgid = ((uint)paddr - APPS_PAGES_BASE) / PAGE_SIZE;
    ASSERT(pgid < APPS_PAGES_CNT, "paddr_to_pgid: pgid is too large");
    return pgid;
}

int tmp_test(uint pgid) {
    return bitmap_test(pgid);
}

void* pmalloc(int clear_page) {
    PMEM_LOCK();
    for (uint i = 0; i < APPS_PAGES_CNT; i++) {
        if (bitmap_test(i) == 0) {
            bitmap_set(i);
            void *ret = pgid_to_paddr(i);
            if (clear_page) {
                memset(ret, 0, PAGE_SIZE);
            }
            PMEM_UNLOCK();
            return ret;
        }
    }
    PMEM_UNLOCK();
    FATAL("pmalloc: no more available page");
}

void pfree(void *paddr) {
    PMEM_LOCK();
    uint pgid = paddr_to_pgid(paddr);
    bitmap_clear(pgid);
    PMEM_UNLOCK();
}

