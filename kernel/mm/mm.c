/**
 * SimpleOS Kernel - Memory Management Implementation
 * Buddy allocator, paging, and virtual memory
 */

#include "mm.h"
#include "kernel_types.h"
#include <string.h>

static buddy_allocator_t kernel_buddy_allocator;
static memory_zone_t memory_zones[3];

/* ============================================================================
 * BUDDY ALLOCATOR IMPLEMENTATION
 * =========================================================================== */

void mm_init(u32 mem_size) {
    memset(&kernel_buddy_allocator, 0, sizeof(buddy_allocator_t));
    
    /* Initialize free lists for each order */
    for (int i = 0; i < BUDDY_ORDER_COUNT; i++) {
        list_init(&kernel_buddy_allocator.free_lists[i]);
    }
    
    kernel_buddy_allocator.total_pages = PAGES_FOR(mem_size);
    kernel_buddy_allocator.free_pages = kernel_buddy_allocator.total_pages;
    kernel_buddy_allocator.base_address = 0x400000;  /* 4MB offset */
    
    /* Initialize first free block as largest power of 2 <= total */
    u32 largest_order = 0;
    u32 largest_size = 1;
    while (largest_size <= kernel_buddy_allocator.total_pages) {
        largest_size <<= 1;
        largest_order++;
    }
    largest_order--; largest_size >>= 1;
    
    /* TODO: Add initial free blocks to appropriate lists */
}

/**
 * Allocate pages using buddy algorithm
 */
addr_t mm_alloc_pages(u32 pages) {
    if (pages == 0 || pages > kernel_buddy_allocator.free_pages) {
        return 0;  /* Allocation failed */
    }
    
    /* Find smallest power-of-2 order that fits */
    u32 order = 0;
    u32 size = 1;
    while (size < pages) {
        size <<= 1;
        order++;
    }
    
    if (order > BUDDY_MAX_ORDER - BUDDY_MIN_ORDER) {
        return 0;  /* Too large */
    }
    
    /* Try to allocate from appropriate order or higher */
    for (u32 curr_order = order; curr_order < BUDDY_ORDER_COUNT; curr_order++) {
        if (!list_empty(&kernel_buddy_allocator.free_lists[curr_order])) {
            /* Found a free block - allocate it */
            list_node_t *block = kernel_buddy_allocator.free_lists[curr_order].head.next;
            list_remove(block);
            
            /* Calculate address */
            addr_t addr = kernel_buddy_allocator.base_address + 
                         (block - (list_node_t *)kernel_buddy_allocator.base_address) * PAGE_SIZE;
            
            kernel_buddy_allocator.allocated_blocks[curr_order]++;
            kernel_buddy_allocator.free_pages -= (1 << (BUDDY_MIN_ORDER + curr_order));
            
            return addr;
        }
    }
    
    return 0;  /* No suitable block found */
}

void mm_free_pages(addr_t addr, u32 pages) {
    if (addr < kernel_buddy_allocator.base_address) {
        return;  /* Invalid address */
    }
    
    /* Find order of allocation */
    u32 order = 0;
    u32 size = 1;
    while (size < pages) {
        size <<= 1;
        order++;
    }
    
    if (order > BUDDY_MAX_ORDER - BUDDY_MIN_ORDER) {
        return;
    }
    
    /* TODO: Buddy coalescing logic */
    /* This would merge adjacent free blocks */
}

/* ============================================================================
 * KERNEL MALLOC/FREE
 * =========================================================================== */

void* kmalloc(u32 size) {
    if (size == 0) return NULL;
    
    u32 pages = PAGES_FOR(size);
    addr_t addr = mm_alloc_pages(pages);
    return (void *)addr;
}

void* kmalloc_aligned(u32 size, u32 alignment) {
    if (size == 0 || !IS_ALIGNED(alignment, alignment))
        return NULL;
    
    u32 alloc_size = size + alignment;
    void *ptr = kmalloc(alloc_size);
    if (!ptr) return NULL;
    
    addr_t aligned = ALIGN((addr_t)ptr, alignment);
    return (void *)aligned;
}

void kfree(void *ptr) {
    if (!ptr) return;
    
    /* TODO: Free the pages that were allocated */
    addr_t addr = (addr_t)ptr;
    u32 pages = 1;  /* Default, should track actual size */
    mm_free_pages(addr, pages);
}

/* ============================================================================
 * VIRTUAL MEMORY / PAGING
 * =========================================================================== */

page_directory_t* vm_create_space(void) {
    page_directory_t *pdir = (page_directory_t *)kmalloc(sizeof(page_directory_t));
    if (!pdir) return NULL;
    
    memset(pdir->pde, 0, sizeof(pdir->pde));
    
    /* Identity map kernel space (first 4MB) */
    for (u32 i = 0; i < 1024; i++) {
        addr_t paddr = i * PAGE_SIZE;
        pdir->pde[i] = (paddr & 0xFFFFF000) | PDE_PRESENT | PDE_WRITABLE;
    }
    
    return pdir;
}

void vm_destroy_space(page_directory_t *pdir) {
    if (!pdir) return;
    
    /* TODO: Free all page tables and pages */
    kfree(pdir);
}

s32 vm_map_page(page_directory_t *pdir, addr_t vaddr, addr_t paddr, u32 flags) {
    if (!pdir || !IS_ALIGNED(vaddr, PAGE_SIZE) || !IS_ALIGNED(paddr, PAGE_SIZE)) {
        return KERNEL_EINVAL;
    }
    
    /* Get page directory entry index */
    u32 pdy_index = vaddr >> 22;
    if (pdy_index >= 1024) return KERNEL_EINVAL;
    
    /* Check if page table exists */
    if (!(pdir->pde[pdy_index] & PDE_PRESENT)) {
        /* Allocate page table */
        addr_t pt_addr = mm_alloc_pages(1);
        if (!pt_addr) return KERNEL_ENOMEM;
        
        pdir->pde[pdy_index] = (pt_addr & 0xFFFFF000) | 
                               PDE_PRESENT | PDE_WRITABLE | PDE_USER;
    }
    
    /* Get page table */
    page_table_t *ptable = (page_table_t *)(pdir->pde[pdy_index] & 0xFFFFF000);
    u32 pte_index = (vaddr >> 12) & 0x3FF;
    
    /* Set page table entry */
    ptable->pte[pte_index] = (paddr & 0xFFFFF000) | flags;
    
    /* TODO: Invalidate TLB */
    asm volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    
    return KERNEL_SUCCESS;
}

void vm_unmap_page(page_directory_t *pdir, addr_t vaddr) {
    if (!pdir || !IS_ALIGNED(vaddr, PAGE_SIZE)) return;
    
    u32 pdy_index = vaddr >> 22;
    if (pdy_index >= 1024) return;
    
    if (!(pdir->pde[pdy_index] & PDE_PRESENT)) return;
    
    page_table_t *ptable = (page_table_t *)(pdir->pde[pdy_index] & 0xFFFFF000);
    u32 pte_index = (vaddr >> 12) & 0x3FF;
    
    ptable->pte[pte_index] = 0;
    
    asm volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
}

addr_t vm_get_phys_addr(page_directory_t *pdir, addr_t vaddr) {
    if (!pdir) return 0;
    
    u32 pdy_index = vaddr >> 22;
    if (pdy_index >= 1024) return 0;
    
    if (!(pdir->pde[pdy_index] & PDE_PRESENT)) return 0;
    
    page_table_t *ptable = (page_table_t *)(pdir->pde[pdy_index] & 0xFFFFF000);
    u32 pte_index = (vaddr >> 12) & 0x3FF;
    
    if (!(ptable->pte[pte_index] & PTE_PRESENT)) return 0;
    
    return (ptable->pte[pte_index] & 0xFFFFF000) | (vaddr & 0xFFF);
}

void vm_activate(page_directory_t *pdir) {
    if (!pdir) return;
    
    /* Load page directory into CR3 */
    addr_t pdir_addr = (addr_t)pdir;
    asm volatile("mov %0, %%cr3" : : "r"(pdir_addr));
    
    /* Enable paging if not already enabled */
    u32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    if (!(cr0 & 0x80000000)) {
        cr0 |= 0x80000000;
        asm volatile("mov %0, %%cr0" : : "r"(cr0));
    }
}

page_directory_t* vm_get_current(void) {
    addr_t pdir;
    asm volatile("mov %%cr3, %0" : "=r"(pdir));
    return (page_directory_t *)pdir;
}

void mm_print_stats(void) {
    printk("=== MEMORY STATS ===\n");
    printk("Total pages: %d\n", kernel_buddy_allocator.total_pages);
    printk("Free pages: %d\n", kernel_buddy_allocator.free_pages);
    printk("Used pages: %d (%.1f%%)\n",
           kernel_buddy_allocator.total_pages - kernel_buddy_allocator.free_pages,
           ((float)(kernel_buddy_allocator.total_pages - kernel_buddy_allocator.free_pages) / 
            kernel_buddy_allocator.total_pages) * 100);
}
