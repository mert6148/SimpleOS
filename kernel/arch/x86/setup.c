/**
 * SimpleOS x86 Architecture Setup
 * Initializes processor features, paging, and exception handlers
 */

#include "kernel_types.h"
#include "sched.h"
#include "mm.h"

/* ============================================================================
 * x86 CONTROL REGISTERS
 * =========================================================================== */

static inline u32 read_cr0(void) {
    u32 val;
    asm volatile("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline void write_cr0(u32 val) {
    asm volatile("mov %0, %%cr0" : : "r"(val));
}

static inline u32 read_cr3(void) {
    u32 val;
    asm volatile("mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void write_cr3(u32 val) {
    asm volatile("mov %0, %%cr3" : : "r"(val));
}

static inline u32 read_cr4(void) {
    u32 val;
    asm volatile("mov %%cr4, %0" : "=r"(val));
    return val;
}

static inline void write_cr4(u32 val) {
    asm volatile("mov %0, %%cr4" : : "r"(val));
}

static inline u32 read_eflags(void) {
    u32 flags;
    asm volatile("pushfl; popl %0" : "=r"(flags));
    return flags;
}

static inline void write_eflags(u32 flags) {
    asm volatile("pushl %0; popfl" : : "r"(flags));
}

/* ============================================================================
 * GLOBAL DESCRIPTOR TABLE
 * =========================================================================== */

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_mid;
    u8 access;
    u8 limit_high_flags;
    u8 base_high;
} __attribute__((packed));

struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

#define GDT_ENTRIES 6
static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gdt_ptr;

void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_mid = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].limit_high_flags = (limit >> 16) & 0x0F;
    gdt[num].limit_high_flags |= (gran & 0xF0);
    
    gdt[num].access = access;
}

void gdt_install(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdt_ptr.base = (u32)&gdt;
    
    /* NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);
    
    /* Kernel code segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    /* Kernel data segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    /* User code segment */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    /* User data segment */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    /* Task state segment (for context switching) */
    /* Simplified for now */
    gdt_set_gate(5, 0, 0, 0, 0);
    
    asm volatile("lgdtl (%0)" : : "r"(&gdt_ptr));
    
    /* Reload segment registers */
    asm volatile("ljmpl $0x08, $1f; 1:");
    asm volatile("mov $0x10, %%ax; mov %%ax, %%ds; mov %%ax, %%es; "
                 "mov %%ax, %%fs; mov %%ax, %%gs; mov %%ax, %%ss" : : : "ax");
}

/* ============================================================================
 * INTERRUPT DESCRIPTOR TABLE
 * =========================================================================== */

struct idt_entry {
    u16 base_lo;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 base_hi;
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

#define IDT_ENTRIES 256
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idt_ptr;

void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install(void) {
    idt_ptr.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idt_ptr.base = (u32)&idt;
    
    /* Clear all entries */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0x08, 0);
    }
    
    /* Install exception handlers (0-31) */
    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, (u32)exception_handler, 0x08, 0x8E);
    }
    
    /* Install IRQ handlers (32-47) */
    for (int i = 32; i < 48; i++) {
        idt_set_gate(i, (u32)irq_handler, 0x08, 0x8E);
    }
    
    /* System call interrupt (int 0x80) */
    idt_set_gate(0x80, (u32)syscall_handler, 0x08, 0xEE);
    
    asm volatile("lidt (%0)" : : "r"(&idt_ptr));
}

/* Exception and interrupt handlers (stubs - implemented elsewhere) */
extern void exception_handler(void);
extern void irq_handler(void);
extern void syscall_handler(void);

/* ============================================================================
 * PAGING SETUP
 * =========================================================================== */

static u32 kernel_pgdir[1024] __attribute__((aligned(4096)));
static u32 kernel_pgtbl[1024] __attribute__((aligned(4096)));

void paging_init(void) {
    /* Clear page directory and table */
    for (int i = 0; i < 1024; i++) {
        kernel_pgdir[i] = 0;
        kernel_pgtbl[i] = 0;
    }
    
    /* Map first 4MB with 4KB pages */
    for (u32 i = 0; i < 1024; i++) {
        kernel_pgtbl[i] = (i * 0x1000) | 0x3;  /* Present + writeable */
    }
    
    /* First entry in page directory points to page table */
    kernel_pgdir[0] = ((u32)kernel_pgtbl) | 0x3;
    
    /* Also map kernel to 0xC0000000 (3GB) */
    kernel_pgdir[768] = ((u32)kernel_pgtbl) | 0x3;
    
    /* Set CR3 to page directory */
    write_cr3((u32)kernel_pgdir);
    
    /* Enable paging */
    u32 cr0 = read_cr0();
    cr0 |= 0x80000000;  /* PG bit */
    write_cr0(cr0);
}

/* ============================================================================
 * CPU FEATURE INITIALIZATION
 * =========================================================================== */

void cpu_init(void) {
    /* Enable PAE (if 64-bit support) */
    u32 cr4 = read_cr4();
    cr4 |= 0x20;  /* PAE bit */
    write_cr4(cr4);
    
    /* Enable Math Coprocessor */
    u32 cr0 = read_cr0();
    cr0 &= ~(1 << 2);  /* Clear EM */
    cr0 |= (1 << 1);   /* Set MP */
    write_cr0(cr0);
    
    /* Clear FPU state */
    asm volatile("fninit");
    
    /* Enable SSE if available */
    asm volatile("mov %%cr4, %%eax; or $0x600, %%eax; mov %%eax, %%cr4" : : : "eax");
}

/* ============================================================================
 * INITIALIZATION ENTRYPOINT
 * =========================================================================== */

void arch_init(void) {
    /* Install GDT */
    gdt_install();
    
    /* Install IDT */
    idt_install();
    
    /* Initialize CPU features */
    cpu_init();
    
    /* Set up paging */
    paging_init();
    
    /* Enable interrupts */
    asm volatile("sti");
}
