/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: wrapping the CPU interface for memory management unit (MMU)
 *
 * Updated by CS6640 26spring staff.
 */

#include "egos.h"
#include <string.h>

/* Interface to allocate/free a physical page */
void *pmalloc(int clear);
void pfree(void *paddr);
uint paddr_to_pgid(void *paddr); // FIXME: remove this later

// FIXME: return physical addr
uint mmu_alloc() {
    void *paddr = pmalloc(1);
    return paddr_to_pgid(paddr); // FIXME
}

void flush_cache() {
    /* QEMU doesn't need L1 instruction cache flush */
    if (earth->translation == PAGE_TABLE) {
        /* Flush the TLB, the cache for page table entries. */
        /* See
         * https://riscv.org/wp-content/uploads/2017/05/riscv-privileged-v1.10.pdf#subsection.4.2.1
         */
        asm("sfence.vma zero,zero");
    }
}

/* defined in mem_soft_tlb.c */
void soft_tlb_free(int pid);
void soft_tlb_map(int pid, uint vpage_no, uint pgid);
void soft_tlb_switch(int pid);
uint soft_tlb_translate(int pid, uint vaddr);

/* defined in mem_vm.c */
void pmp_init();
void vm_init();

void mmu_init() {
    earth->mmu_alloc       = mmu_alloc;
    earth->mmu_flush_cache = flush_cache;

    earth->translation   = SOFT_TLB;
    earth->mmu_free      = soft_tlb_free;
    earth->mmu_map       = soft_tlb_map;
    earth->mmu_switch    = soft_tlb_switch;
    earth->mmu_translate = soft_tlb_translate;
}
