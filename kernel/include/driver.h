/**
 * SimpleOS Kernel - Device Driver Framework
 * Provides driver model, device registration, and hardware abstraction
 */

#ifndef KERNEL_DRIVER_H
#define KERNEL_DRIVER_H

#include "kernel_types.h"

/* ============================================================================
 * DEVICE TYPES
 * =========================================================================== */

typedef enum {
    DEVICE_CLASS_UNKNOWN = 0,
    DEVICE_CLASS_STORAGE = 1,
    DEVICE_CLASS_NETWORK = 2,
    DEVICE_CLASS_INPUT = 3,
    DEVICE_CLASS_OUTPUT = 4,
    DEVICE_CLASS_MISC = 5
} device_class_t;

/* ============================================================================
 * DRIVER INTERFACE
 * =========================================================================== */

typedef struct driver {
    char name[32];
    char version[16];
    device_class_t device_class;
    
    /* Lifecycle callbacks */
    s32 (*probe)(struct device *dev);
    s32 (*remove)(struct device *dev);
    s32 (*suspend)(struct device *dev);
    s32 (*resume)(struct device *dev);
    
    /* Custom data */
    void *priv;
} driver_t;

/* ============================================================================
 * DEVICE STRUCTURE
 * =========================================================================== */

typedef struct device {
    dev_t dev_num;                      /* Device number (major, minor) */
    char name[32];                      /* Device name */
    char path[256];                     /* Device path (/dev/xxx) */
    device_class_t device_class;
    
    /* Driver reference */
    driver_t *driver;
    
    /* Device operations */
    struct device_ops *ops;
    
    /* Hardware info */
    addr_t io_base;                     /* I/O port or memory base */
    u32 io_size;
    u32 irq;                            /* Interrupt number */
    
    /* State */
    bool enabled;
    bool available;
    
    /* Private driver data */
    void *priv;
    
    /* Linked list */
    list_node_t node;
} device_t;

/* ============================================================================
 * DEVICE OPERATIONS
 * =========================================================================== */

typedef struct device_ops {
    /* Basic operations */
    s32 (*open)(device_t *dev);
    s32 (*close)(device_t *dev);
    s32 (*read)(device_t *dev, u32 offset, void *buf, u32 size);
    s32 (*write)(device_t *dev, u32 offset, const void *buf, u32 size);
    
    /* Control operations */
    s32 (*ioctl)(device_t *dev, u32 cmd, void *arg);
    s32 (*mmap)(device_t *dev, addr_t vaddr, u32 offset, u32 size);
    
    /* Interrupt handling */
    void (*irq_handler)(device_t *dev, u32 irq);
} device_ops_t;

/* ============================================================================
 * INTERRUPT HANDLER
 * =========================================================================== */

typedef void (*irq_handler_t)(u32 irq, void *data);

typedef struct {
    irq_handler_t handler;
    void *data;
    device_t *device;
} irq_entry_t;

/* ============================================================================
 * DRIVER REGISTRATION & MANAGEMENT
 * =========================================================================== */

/**
 * Initialize driver framework
 */
void driver_init(void);

/**
 * Register a driver
 * @param driver Driver structure to register
 * @return 0 on success, negative on error
 */
s32 driver_register(driver_t *driver);

/**
 * Unregister a driver
 */
s32 driver_unregister(driver_t *driver);

/**
 * Register a device
 * @param dev Device to register
 * @param driver Driver for this device
 * @return Device number or error
 */
dev_t device_register(device_t *dev, driver_t *driver);

/**
 * Unregister a device
 */
s32 device_unregister(dev_t dev_num);

/**
 * Get device by name
 */
device_t* device_get_by_name(const char *name);

/**
 * Get device by number
 */
device_t* device_get_by_num(dev_t dev_num);

/**
 * Enable a device
 */
s32 device_enable(device_t *dev);

/**
 * Disable a device
 */
s32 device_disable(device_t *dev);

/* ============================================================================
 * INTERRUPT MANAGEMENT
 * =========================================================================== */

/**
 * Register interrupt handler
 * @param irq Interrupt number
 * @param handler Handler function
 * @param dev Associated device (optional)
 * @return 0 on success
 */
s32 irq_register(u32 irq, irq_handler_t handler, device_t *dev);

/**
 * Unregister interrupt handler
 */
s32 irq_unregister(u32 irq);

/**
 * Dispatch interrupt
 * Called by low-level interrupt handler
 */
void irq_dispatch(u32 irq);

/**
 * Acknowledge interrupt
 */
void irq_ack(u32 irq);

/**
 * Enable interrupt
 */
void irq_enable(u32 irq);

/**
 * Disable interrupt
 */
void irq_disable(u32 irq);

/* ============================================================================
 * I/O OPERATIONS
 * =========================================================================== */

/* I/O Port operations */
static INLINE u8 inb(u16 port) {
    u8 value;
    asm volatile("inb %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static INLINE u16 inw(u16 port) {
    u16 value;
    asm volatile("inw %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static INLINE u32 inl(u16 port) {
    u32 value;
    asm volatile("inl %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static INLINE void outb(u16 port, u8 value) {
    asm volatile("outb %0, %1" : : "a"(value), "d"(port));
}

static INLINE void outw(u16 port, u16 value) {
    asm volatile("outw %0, %1" : : "a"(value), "d"(port));
}

static INLINE void outl(u16 port, u32 value) {
    asm volatile("outl %0, %1" : : "a"(value), "d"(port));
}

/* Memory-mapped I/O */
static INLINE u8 mmio_read8(volatile addr_t addr) {
    return *(volatile u8 *)addr;
}

static INLINE u16 mmio_read16(volatile addr_t addr) {
    return *(volatile u16 *)addr;
}

static INLINE u32 mmio_read32(volatile addr_t addr) {
    return *(volatile u32 *)addr;
}

static INLINE void mmio_write8(volatile addr_t addr, u8 value) {
    *(volatile u8 *)addr = value;
}

static INLINE void mmio_write16(volatile addr_t addr, u16 value) {
    *(volatile u16 *)addr = value;
}

static INLINE void mmio_write32(volatile addr_t addr, u32 value) {
    *(volatile u32 *)addr = value;
}

#endif /* KERNEL_DRIVER_H */
