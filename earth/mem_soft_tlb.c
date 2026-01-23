/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: software TLB for memory translation
 *
 * Updated by CS6640 26spring staff.
 */

#include "egos.h"
#include <string.h>


#define PAGE_NO_TO_ADDR(x) (char*)(x * PAGE_SIZE)
/* page id starts from APPS_PAGES_BASE */
#define PAGE_ID_TO_ADDR(x) ((char*)APPS_PAGES_BASE + x * PAGE_SIZE)

/* Interface to allocate/free a physical page */
void* pmalloc(int clear_page);
void  pfree(void *paddr);

/* managing physical pages */
struct page {
    int pid;
    uint vpage_no;
} page_info_table[APPS_PAGES_CNT];


void soft_tlb_free(int pid) {
    for (uint i = 0; i < APPS_PAGES_CNT; i++) {
        if (page_info_table[i].pid == pid) {
            page_info_table[i].pid = 0;
            pfree(PAGE_ID_TO_ADDR(i));
        }
    }
}

void soft_tlb_map(int pid, uint vpage_no, uint ppage_id) {
    page_info_table[ppage_id].pid      = pid;
    page_info_table[ppage_id].vpage_no = vpage_no;
}

void soft_tlb_switch(int pid) {
    static int curr_vm_pid = -1;
    if (pid == curr_vm_pid) return;

    /* Unmap curr_vm_pid from the user address space. */
    for (uint i = 0; i < APPS_PAGES_CNT; i++) {
        if (page_info_table[i].pid == curr_vm_pid) {
            memcpy(PAGE_ID_TO_ADDR(i),
                   PAGE_NO_TO_ADDR(page_info_table[i].vpage_no), PAGE_SIZE);
        }
    }

    /* Map pid to the user address space. */
    for (uint i = 0; i < APPS_PAGES_CNT; i++)
        if (page_info_table[i].pid == pid)
            memcpy(PAGE_NO_TO_ADDR(page_info_table[i].vpage_no),
                   PAGE_ID_TO_ADDR(i), PAGE_SIZE);

    curr_vm_pid = pid;
}

uint soft_tlb_translate(int pid, uint vaddr) {
    soft_tlb_switch(pid);
    return vaddr;
}

