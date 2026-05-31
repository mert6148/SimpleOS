#include <stdint.h>
#include <stddef.h>

#ifndef SIMPLEOS_IDT_H
#define SIMPLEOS_IDT_H

/* IDT entry structure (64-bit) */
struct idt_entry {
    uint16_t base_lo;    /* Lower 16 bits of handler function address */
    uint16_t sel;        /* Kernel segment selector */
    uint8_t always0;     /* This must be zero */
    uint8_t flags;       /* Type and attributes */
    uint16_t base_hi;    /* Upper 16 bits of handler function address */
} __attribute__((packed));

/ * IDT pointer structure */
struct idt_ptr {
    uint16_t limit;      /* Size of IDT - 1 */
    uint64_t base;       /* Base address of IDT */
} __attribute__((packed));