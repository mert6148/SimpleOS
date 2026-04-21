/**
 * SimpleOS Kernel - Main Entry Point
 * Initializes all kernel subsystems and starts the first user task
 */

#include "kernel_types.h"
#include "sched.h"
#include "mm.h"
#include "fs.h"
#include "driver.h"
#include <stdarg.h>
#include <stdio.h>

/* ============================================================================
 * SERIAL CONSOLE / PRINTK
 * =========================================================================== */

#define SERIAL_PORT 0x3F8

void serial_putchar(char c) {
    /* Wait for transmit buffer empty */
    while (!(inb(SERIAL_PORT + 5) & 0x20))
        ;
    outb(SERIAL_PORT, c);
}

void printk(const char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    for (const char *p = buffer; *p; p++) {
        if (*p == '\n') {
            serial_putchar('\r');
        }
        serial_putchar(*p);
    }
}

/* ============================================================================
 * KERNEL INITIALIZATION
 * =========================================================================== */

/**
 * Kernel's main function - called from bootloader
 * This is where the OS really starts
 */
void kernel_main(u32 magic, u32 mbinfo) {
    /* Initialize early console for debugging output */
    printk("\n");
    printk("╔════════════════════════════════════════════════════════════╗\n");
    printk("║         SimpleOS Kernel v1.0.0 - Starting...              ║\n");
    printk("║         Building the Central Brain...                     ║\n");
    printk("╚════════════════════════════════════════════════════════════╝\n");
    printk("\n");
    
    /* ====== PHASE 1: MEMORY SETUP ====== */
    printk("[KERNEL] 🔧 Phase 1: Setting up memory management...\n");
    mm_init(128 * 1024 * 1024);  /* 128MB RAM */
    printk("[KERNEL] ✓ Memory manager initialized\n");
    mm_print_stats();
    
    /* ====== PHASE 2: SCHEDULER ====== */
    printk("\n[KERNEL] 🔧 Phase 2: Initializing process scheduler...\n");
    sched_init();
    printk("[KERNEL] ✓ Scheduler initialized\n");
    
    /* ====== PHASE 3: FILE SYSTEM ====== */
    printk("\n[KERNEL] 🔧 Phase 3: Initializing virtual file system...\n");
    vfs_init();
    printk("[KERNEL] ✓ Virtual file system initialized\n");
    
    /* ====== PHASE 4: DRIVER FRAMEWORK ====== */
    printk("\n[KERNEL] 🔧 Phase 4: Setting up device driver framework...\n");
    driver_init();
    printk("[KERNEL] ✓ Driver framework initialized\n");
    
    /* ====== PHASE 5: SYSTEM SETUP ====== */
    printk("\n[KERNEL] 🔧 Phase 5: Setting up system features...\n");
    
    /* Create data structures for virtual memory */
    page_directory_t *kernel_space = vm_create_space();
    if (kernel_space) {
        vm_activate(kernel_space);
        printk("[KERNEL] ✓ Virtual memory enabled\n");
    } else {
        printk("[KERNEL] ⚠ Virtual memory setup failed\n");
    }
    
    /* ====== PHASE 6: FIRST TASK ====== */
    printk("\n[KERNEL] 🔧 Phase 6: Creating first user task...\n");
    
    /* Create init task (PID 1) */
    pid_t init_pid = task_create(&init_task, SCHED_PRIORITY_NORMAL);
    if (init_pid > 0) {
        printk("[KERNEL] ✓ Init task created (PID %d)\n", init_pid);
    } else {
        printk("[KERNEL] ✗ Failed to create init task!\n");
        kernel_panic("Cannot create init task");
    }
    
    /* ====== BOOT COMPLETE ====== */
    printk("\n");
    printk("╔════════════════════════════════════════════════════════════╗\n");
    printk("║            ✓ SimpleOS Kernel Boot Complete                ║\n");
    printk("║            The Central Brain is Ready!                    ║\n");
    printk("╚════════════════════════════════════════════════════════════╝\n");
    printk("\n");
    
    /* Start task scheduler */
    printk("[KERNEL] Starting scheduler...\n");
    sched_yield();
    
    /* Should not reach here */
    kernel_panic("Scheduler returned to kernel_main!");
}

/* ============================================================================
 * INIT TASK - Program executed by first user task
 * =========================================================================== */

void init_task(void) {
    printk("\n[INIT] Init task started (PID %d)\n", 
           sched_get_current()->pid);
    
    /* Create some example child tasks */
    printk("[INIT] Creating child tasks...\n");
    
    task_create(&worker_task_1, SCHED_PRIORITY_NORMAL);
    task_create(&worker_task_2, SCHED_PRIORITY_HIGH);
    
    printk("[INIT] Child tasks created, yielding CPU...\n");
    
    /* Wait for a while */
    for (u32 i = 0; i < 1000000000; i++) {
        asm volatile("nop");
    }
    
    printk("[INIT] Init task exiting\n");
    task_exit(0);
}

void worker_task_1(void) {
    task_t *current = sched_get_current();
    printk("[WORKER-1] Task started (PID %d, priority %d)\n",
           current->pid, current->priority);
    
    for (int i = 0; i < 5; i++) {
        printk("[WORKER-1] Iteration %d\n", i);
        sched_yield();
    }
    
    printk("[WORKER-1] Task exiting\n");
    task_exit(0);
}

void worker_task_2(void) {
    task_t *current = sched_get_current();
    printk("[WORKER-2] Task started (PID %d, priority %d)\n",
           current->pid, current->priority);
    
    for (int i = 0; i < 5; i++) {
        printk("[WORKER-2] Iteration %d\n", i);
        sched_yield();
    }
    
    printk("[WORKER-2] Task exiting\n");
    task_exit(0);
}

/* ============================================================================
 * ERROR HANDLING
 * =========================================================================== */

void kernel_panic(const char *fmt, ...) {
    printk("\n");
    printk("╔════════════════════════════════════════════════════════════╗\n");
    printk("║                   KERNEL PANIC!                           ║\n");
    printk("╚════════════════════════════════════════════════════════════╝\n");
    
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    printk("Error: %s\n", buffer);
    
    /* Hang system */
    printk("System halted.\n");
    while (1) {
        asm("hlt");
    }
}

int sprintf(char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(str, 4096, fmt, args);
    va_end(args);
    return ret;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    int i = 0;
    
    while (*fmt && i < (int)size - 1) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 'd') {
                int val = va_arg(ap, int);
                i += sprintf(str + i, "%d", val);
            } else if (*fmt == 'x') {
                unsigned val = va_arg(ap, unsigned);
                i += sprintf(str + i, "%x", val);
            } else if (*fmt == 's') {
                const char *s = va_arg(ap, const char *);
                while (*s && i < (int)size - 1) {
                    str[i++] = *s++;
                }
            } else if (*fmt == 'c') {
                char c = (char)va_arg(ap, int);
                str[i++] = c;
            } else if (*fmt == '%') {
                str[i++] = '%';
            }
            fmt++;
        } else {
            str[i++] = *fmt++;
        }
    }
    
    str[i] = '\0';
    return i;
}

/* ============================================================================
 * STUB FUNCTIONS (For now)
 * =========================================================================== */

void irq_handler_stub(u32 irq) {
    printk("[KERNEL] IRQ %d received\n", irq);
}

void exception_handler_stub(u32 exception) {
    kernel_panic("Unhandled exception: %d", exception);
}
