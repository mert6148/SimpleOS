/**
 * SimpleOS Programmable Interrupt Controller Driver
 * Manages 8259 PIC for IRQ handling on x86
 */

#include "kernel_types.h"
#include "driver.h"

#define PIC1_COMMAND   0x20
#define PIC1_DATA      0x21
#define PIC2_COMMAND   0xA0
#define PIC2_DATA      0xA1

/* IRQ handler function pointers */
static irq_handler_t irq_handlers[16];

/**
 * Initialize the 8259 Programmable Interrupt Controller
 */
s32 pic_init(void) {
    /* Clear handler array */
    for (int i = 0; i < 16; i++) {
        irq_handlers[i] = NULL;
    }
    
    /* ICW1: Initialize Command Word 1 */
    outb(PIC1_COMMAND, 0x11);  /* ICW1: 0001 0001 - edge triggered, cascade, ICW4 needed */
    outb(PIC2_COMMAND, 0x11);
    
    /* ICW2: Set interrupt vector base */
    outb(PIC1_DATA, 0x20);     /* Master PIC at vector 0x20 (IRQ0-7 map to int 0x20-0x27) */
    outb(PIC2_DATA, 0x28);     /* Slave PIC at vector 0x28 (IRQ8-15 map to int 0x28-0x2F) */
    
    /* ICW3: Set up cascading */
    outb(PIC1_DATA, 0x04);     /* Master IR2 (0000 0100) connects to slave */
    outb(PIC2_DATA, 0x02);     /* Slave connected to master IR2 (0000 0010) */
    
    /* ICW4: Set environment */
    outb(PIC1_DATA, 0x01);     /* 8086 mode (not 8080) */
    outb(PIC2_DATA, 0x01);
    
    /* OCW1: Set interrupt masks - enable all IRQs initially */
    outb(PIC1_DATA, 0x00);     /* Master: enable all (0000 0000) */
    outb(PIC2_DATA, 0x00);     /* Slave: enable all (0000 0000) */
    
    return KERNEL_SUCCESS;
}

/**
 * Register an IRQ handler
 */
s32 pic_register_irq(u8 irq, irq_handler_t handler) {
    if (irq >= 16) {
        return KERNEL_EINVAL;
    }
    
    irq_handlers[irq] = handler;
    
    /* Enable IRQ in PIC */
    return pic_enable_irq(irq);
}

/**
 * Unregister an IRQ handler
 */
s32 pic_unregister_irq(u8 irq) {
    if (irq >= 16) {
        return KERNEL_EINVAL;
    }
    
    irq_handlers[irq] = NULL;
    
    return pic_disable_irq(irq);
}

/**
 * Get registered handler for an IRQ
 */
irq_handler_t pic_get_irq_handler(u8 irq) {
    if (irq >= 16) {
        return NULL;
    }
    
    return irq_handlers[irq];
}

/**
 * Enable a specific IRQ
 */
s32 pic_enable_irq(u8 irq) {
    if (irq >= 16) {
        return KERNEL_EINVAL;
    }
    
    if (irq < 8) {
        /* Master PIC */
        u8 mask = inb(PIC1_DATA);
        mask &= ~(1 << irq);    /* Clear mask bit */
        outb(PIC1_DATA, mask);
    } else {
        /* Slave PIC */
        u8 mask = inb(PIC2_DATA);
        mask &= ~(1 << (irq - 8));
        outb(PIC2_DATA, mask);
        
        /* Also enable cascade line on master */
        u8 master_mask = inb(PIC1_DATA);
        master_mask &= ~(1 << 2);  /* Enable IR2 */
        outb(PIC1_DATA, master_mask);
    }
    
    return KERNEL_SUCCESS;
}

/**
 * Disable a specific IRQ
 */
s32 pic_disable_irq(u8 irq) {
    if (irq >= 16) {
        return KERNEL_EINVAL;
    }
    
    if (irq < 8) {
        /* Master PIC */
        u8 mask = inb(PIC1_DATA);
        mask |= (1 << irq);     /* Set mask bit */
        outb(PIC1_DATA, mask);
    } else {
        /* Slave PIC */
        u8 mask = inb(PIC2_DATA);
        mask |= (1 << (irq - 8));
        outb(PIC2_DATA, mask);
    }
    
    return KERNEL_SUCCESS;
}

/**
 * Send EOI (End Of Interrupt) to PIC
 */
void pic_eoi(u8 irq) {
    const u8 EOI = 0x20;
    
    if (irq >= 8) {
        /* Slave IRQ - send EOI to slave first */
        outb(PIC2_COMMAND, EOI);
    }
    
    /* Always send to master */
    outb(PIC1_COMMAND, EOI);
}

/**
 * Get IRQ status from PIC
 * Returns bitmask of which IRQs are asserted
 */
u16 pic_get_status(void) {
    u8 master_status = inb(PIC1_COMMAND);
    u8 slave_status = inb(PIC2_COMMAND);
    
    return ((u16)slave_status << 8) | master_status;
}

/**
 * Get IRQ mask from PIC
 * Returns bitmask of which IRQs are masked (disabled)
 */
u16 pic_get_mask(void) {
    u8 master_mask = inb(PIC1_DATA);
    u8 slave_mask = inb(PIC2_DATA);
    
    return ((u16)slave_mask << 8) | master_mask;
}
