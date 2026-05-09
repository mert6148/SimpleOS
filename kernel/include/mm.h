/**
 * SimpleOS Kernel - Memory Management
 * Implements physical memory allocation, paging, and virtual memory
 */

#ifndef KERNEL_MM_H
#define KERNEL_MM_H

#include "kernel_types.h"

/* ============================================================================
 * PAGE SYSTEM
 * =========================================================================== */

#define PAGE_SIZE           4096        /* 4KB pages */
#define PAGE_SHIFT          12
#define PAGE_MASK           (~(PAGE_SIZE - 1))
#define PAGE_TO_ADDR(p)     ((p) << PAGE_SHIFT)
#define ADDR_TO_PAGE(a)     ((a) >> PAGE_SHIFT)
#define PAGES_FOR(s)        ALIGN(s, PAGE_SIZE) / PAGE_SIZE

/* ============================================================================
 * MEMORY ALLOCATOR - BUDDY ALGORITHM
 * =========================================================================== */

#define BUDDY_MIN_ORDER     4           /* Minimum allocation power of 2 */
#define BUDDY_MAX_ORDER     20          /* Maximum allocation power of 2 */
#define BUDDY_ORDER_COUNT   (BUDDY_MAX_ORDER - BUDDY_MIN_ORDER + 1)

typedef struct {
    list_t free_lists[BUDDY_ORDER_COUNT];
    u32 allocated_blocks[BUDDY_ORDER_COUNT];
    addr_t base_address;
    u32 total_pages;
    u32 free_pages;
} buddy_allocator_t;

/* ============================================================================
 * VIRTUAL MEMORY / PAGING
 * =========================================================================== */

#define VADDR_BITS          32
#define PADDR_BITS          32

typedef struct {
    u32 pde[1024];          /* Page Directory Entries (1024 entries = 4MB each) */
} page_directory_t;

typedef struct {
    u32 pte[1024];          /* Page Table Entries */
} page_table_t;

/* Page Directory Entry flags (x86) */
#define PDE_PRESENT         0x001
#define PDE_WRITABLE        0x002
#define PDE_USER            0x004
#define PDE_WRITETHROUGH    0x008
#define PDE_CACHE_DISABLE   0x010

/* Page Table Entry flags (x86) */
#define PTE_PRESENT         0x001
#define PTE_WRITABLE        0x002
#define PTE_USER            0x004
#define PTE_WRITETHROUGH    0x008
#define PTE_CACHE_DISABLE   0x010
#define PTE_ACCESSED        0x020
#define PTE_DIRTY           0x040

/* ============================================================================
 * MEMORY ZONE MANAGEMENT
 * =========================================================================== */

typedef enum {
    ZONE_DMA = 0,           /* DMA-safe memory (< 16MB) */
    ZONE_NORMAL = 1,        /* Regular memory */
    ZONE_HIGHMEM = 2        /* High memory */
} zone_type_t;

typedef struct {
    addr_t start;
    u32 size;
    u32 free_pages;
    list_t free_block_list;
} memory_zone_t;

/* ============================================================================
 * MEMORY ALLOCATION INTERFACE
 * =========================================================================== */

/**
 * Initialize memory management
 * @param mem_size Total system memory in bytes
 */
void mm_init(u32 mem_size);

/**
 * Allocate contiguous physical pages
 * @param pages Number of pages to allocate
 * @return Physical address or NULL
 */
addr_t mm_alloc_pages(u32 pages);

/**
 * Free allocated pages
 * @param addr Physical address to free
 * @param pages Number of pages
 */
void mm_free_pages(addr_t addr, u32 pages);

/**
 * Allocate memory in kernel space
 * @param size Bytes to allocate
 * @return Virtual address or NULL
 */
void* kmalloc(u32 size);

/**
 * Allocate aligned memory
 */
void* kmalloc_aligned(u32 size, u32 alignment);

/**
 * Free kernel memory
 */
void kfree(void *ptr);

/**
 * Create a new page directory (virtual address space)
 * @return Address of new page directory
 */
page_directory_t* vm_create_space(void);

/**
 * Destroy a page directory and free associated memory
 */
void vm_destroy_space(page_directory_t *pdir);

/**
 * Map a virtual address to physical address in page directory
 * @param pdir Page directory
 * @param vaddr Virtual address
 * @param paddr Physical address
 * @param flags PTE flags
 */
s32 vm_map_page(page_directory_t *pdir, addr_t vaddr, addr_t paddr, u32 flags);

/**
 * Unmap a virtual address
 */
void vm_unmap_page(page_directory_t *pdir, addr_t vaddr);

/**
 * Get physical address for virtual address
 */
addr_t vm_get_phys_addr(page_directory_t *pdir, addr_t vaddr);

/**
 * Activate a page directory
 */
void vm_activate(page_directory_t *pdir);

/**
 * Get current page directory
 */
page_directory_t* vm_get_current(void);

/**
 * Memory statistics
 */
void mm_print_stats(void);

#endif /* KERNEL_MM_H */
