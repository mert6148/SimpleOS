/**
 * SimpleOS x86 IRQ Handlers
 * Manages hardware interrupt requests from devices
 */

#include "kernel_types.h"
#include "driver.h"

#define PIC1_COMMAND   0x20
#define PIC1_DATA      0x21
#define PIC2_COMMAND   0xA0
#define PIC2_DATA      0xA1

#define PIC_EOI        0x20

/* ============================================================================
 * IRQ DISPATCHER
 * =========================================================================== */

/**
 * Main IRQ handler dispatcher
 * Called from assembly stub when hardware interrupt occurs
 */
void irq_dispatch(u32 irq_num) {
    if (irq_num < 16) {
        /* Call registered handler */
        irq_handler_t handler = driver_get_irq_handler(irq_num);
        if (handler) {
            handler(irq_num);
        }
        
        /* Send EOI to PIC */
        if (irq_num >= 8) {
            /* Cascade: send EOI to slave PIC */
            outb(PIC2_COMMAND, PIC_EOI);
        }
        
        /* Always send EOI to master PIC */
        outb(PIC1_COMMAND, PIC_EOI);
    }
}

/* ============================================================================
 * SPECIFIC IRQ HANDLERS
 * =========================================================================== */

void irq_timer(void) {
    /* Called by PIT (Programmable Interval Timer) */
    /* About 18.2 times per second (legacy PC timer) */
    
    /* Invoke scheduler tick */
    extern void sched_tick(void);
    sched_tick();
}

void irq_keyboard(void) {
    /* Scan code available at port 0x60 */
    u8 scancode = inb(0x60);
    
    /* TODO: Queue keyboard event */
}

void irq_serial(void) {
    /* Serial port data available */
    /* Read and buffer character */
    u8 ch = inb(0x3F8);
    
    /* TODO: Handle serial input */
}

void irq_disk(void) {
    /* Disk controller ready */
    /* TODO: Complete pending disk operation */
}

/* ============================================================================
 * EXCEPTION HANDLERS
 * =========================================================================== */

/**
 * Exception handler dispatcher
 * Called from assembly stub when CPU exception occurs
 */
void exception_dispatch(u32 exc_num, u32 error_code) {
    extern void kernel_panic(const char *fmt, ...);
    extern void printk(const char *fmt, ...);
    
    printk("\n[CPU EXCEPTION]\n");
    printk("Exception: %d (0x%x)\n", exc_num, exc_num);
    printk("Error Code: 0x%x\n", error_code);
    
    switch (exc_num) {
        case 0:  /* Divide by zero */
            kernel_panic("Division by zero");
            break;
        case 1:  /* Single step */
            kernel_panic("Debug exception");
            break;
        case 2:  /* NMI */
            kernel_panic("Non-maskable interrupt");
            break;
        case 3:  /* Breakpoint */
            printk("Breakpoint hit\n");
            break;
        case 4:  /* Overflow */
            kernel_panic("Overflow");
            break;
        case 5:  /* Bounds check */
            kernel_panic("Bounds exceeded");
            break;
        case 6:  /* Invalid opcode */
            kernel_panic("Invalid opcode");
            break;
        case 7:  /* Coprocessor not available */
            kernel_panic("FPU not available");
            break;
        case 8:  /* Double fault */
            kernel_panic("DOUBLE FAULT - System failure");
            break;
        case 9:  /* Coprocessor overrun */
            kernel_panic("Coprocessor overrun");
            break;
        case 10: /* Invalid TSS */
            kernel_panic("Invalid Task State Segment");
            break;
        case 11: /* Segment not present */
            kernel_panic("Segment not present");
            break;
        case 12: /* Stack exception */
            kernel_panic("Stack fault");
            break;
        case 13: /* General protection fault */
            kernel_panic("General protection fault at 0x%x", error_code);
            break;
        case 14: /* Page fault */
            handle_page_fault(error_code);
            break;
        case 15: /* Reserved */
            kernel_panic("Reserved exception");
            break;
        case 16: /* FPU error */
            kernel_panic("Floating point exception");
            break;
        case 17: /* Alignment check */
            kernel_panic("Alignment check failed");
            break;
        default:
            kernel_panic("Unknown exception %d", exc_num);
    }
}

void handle_page_fault(u32 error_code) {
    extern void printk(const char *fmt, ...);
    extern void kernel_panic(const char *fmt, ...);
    
    u32 faulting_addr;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_addr));
    
    printk("[PAGE FAULT] Address: 0x%x\n", faulting_addr);
    printk("  Error code: 0x%x\n", error_code);
    
    if (error_code & 0x1) {
        printk("  Reason: Protection violation\n");
    } else {
        printk("  Reason: Page not present\n");
    }
    
    if (error_code & 0x2) {
        printk("  Access: Write\n");
    } else {
        printk("  Access: Read\n");
    }
    
    if (error_code & 0x4) {
        printk("  Mode: User\n");
    } else {
        printk("  Mode: Kernel\n");
    }
    
    /* Try to allocate more memory or handle demand paging */
    extern s32 vm_handle_fault(u32 addr);
    if (vm_handle_fault(faulting_addr) == 0) {
        return;  /* Handled */
    }
    
    kernel_panic("Unhandled page fault at 0x%x", faulting_addr);
}

/* ============================================================================
 * INTERRUPT SETUP
 * =========================================================================== */

void pic_init(void) {
    /* Initialize 8259 Programmable Interrupt Controller */
    
    /* ICW1: Init command */
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    /* ICW2: Vector offset */
    outb(PIC1_DATA, 0x20);    /* Master IRQs at 0x20-0x27 */
    outb(PIC2_DATA, 0x28);    /* Slave IRQs at 0x28-0x2F */
    
    /* ICW3: Cascade mode */
    outb(PIC1_DATA, 0x04);    /* IR2 has slave */
    outb(PIC2_DATA, 0x02);    /* Connected to master IR2 */
    
    /* ICW4: Environment control */
    outb(PIC1_DATA, 0x01);    /* 8086 mode */
    outb(PIC2_DATA, 0x01);
    
    /* OCW1: Interrupt mask */
    outb(PIC1_DATA, 0x00);    /* Enable all on master */
    outb(PIC2_DATA, 0x00);    /* Enable all on slave */
}

void pit_init(void) {
    /* Initialize 8254 Programmable Interval Timer */
    
    u16 divisor = 18;  /* ~1000 Hz (1ms ticks) */
    
    /* Access command: counter 0, LSB then MSB, mode 3 */
    outb(0x43, 0x34);
    
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void irqs_init(void) {
    pic_init();
    pit_init();
    
    /* Register timer handler */
    driver_register_irq(0, irq_timer);
}
