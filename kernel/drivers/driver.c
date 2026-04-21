/**
 * SimpleOS Kernel - Device Driver Framework Implementation
 */

#include "driver.h"
#include "mm.h"
#include <string.h>

#define MAX_DRIVERS     64
#define MAX_DEVICES     256
#define MAX_IRQ_ENTRIES 16

static driver_t *registered_drivers[MAX_DRIVERS];
static device_t *registered_devices[MAX_DEVICES];
static irq_entry_t irq_handlers[256];

static u32 next_device_num = 0;

/* ============================================================================
 * DRIVER INITIALIZATION & REGISTRATION
 * =========================================================================== */

void driver_init(void) {
    memset(registered_drivers, 0, sizeof(registered_drivers));
    memset(registered_devices, 0, sizeof(registered_devices));
    memset(irq_handlers, 0, sizeof(irq_handlers));
}

s32 driver_register(driver_t *driver) {
    if (!driver) return KERNEL_EINVAL;
    
    /* Find free slot */
    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (!registered_drivers[i]) {
            registered_drivers[i] = driver;
            printk("Driver: Registered driver '%s' v%s\n", 
                   driver->name, driver->version);
            return KERNEL_SUCCESS;
        }
    }
    
    return KERNEL_EBUSY;
}

s32 driver_unregister(driver_t *driver) {
    if (!driver) return KERNEL_EINVAL;
    
    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (registered_drivers[i] == driver) {
            registered_drivers[i] = NULL;
            printk("Driver: Unregistered driver '%s'\n", driver->name);
            return KERNEL_SUCCESS;
        }
    }
    
    return KERNEL_ENOENT;
}

/* ============================================================================
 * DEVICE REGISTRATION
 * =========================================================================== */

dev_t device_register(device_t *dev, driver_t *driver) {
    if (!dev || !driver) return KERNEL_EINVAL;
    
    /* Find free slot */
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (!registered_devices[i]) {
            dev->dev_num = next_device_num++;
            dev->driver = driver;
            registered_devices[i] = dev;
            
            /* Call probe callback */
            if (driver->probe) {
                s32 ret = driver->probe(dev);
                if (ret != KERNEL_SUCCESS) {
                    registered_devices[i] = NULL;
                    return ret;
                }
            }
            
            printk("Driver: Device '%s' registered (dev %d)\n", 
                   dev->name, dev->dev_num);
            return dev->dev_num;
        }
    }
    
    return KERNEL_EBUSY;
}

s32 device_unregister(dev_t dev_num) {
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (registered_devices[i] && registered_devices[i]->dev_num == dev_num) {
            device_t *dev = registered_devices[i];
            
            /* Call remove callback */
            if (dev->driver && dev->driver->remove) {
                dev->driver->remove(dev);
            }
            
            registered_devices[i] = NULL;
            printk("Driver: Device unregistered (dev %d)\n", dev_num);
            return KERNEL_SUCCESS;
        }
    }
    
    return KERNEL_ENOENT;
}

device_t* device_get_by_name(const char *name) {
    if (!name) return NULL;
    
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (registered_devices[i] && 
            strcmp(registered_devices[i]->name, name) == 0) {
            return registered_devices[i];
        }
    }
    
    return NULL;
}

device_t* device_get_by_num(dev_t dev_num) {
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (registered_devices[i] && 
            registered_devices[i]->dev_num == dev_num) {
            return registered_devices[i];
        }
    }
    
    return NULL;
}

/* ============================================================================
 * DEVICE CONTROL
 * =========================================================================== */

s32 device_enable(device_t *dev) {
    if (!dev) return KERNEL_EINVAL;
    
    if (dev->ops && dev->ops->open) {
        s32 ret = dev->ops->open(dev);
        if (ret != KERNEL_SUCCESS) return ret;
    }
    
    dev->enabled = true;
    printk("Driver: Device '%s' enabled\n", dev->name);
    return KERNEL_SUCCESS;
}

s32 device_disable(device_t *dev) {
    if (!dev) return KERNEL_EINVAL;
    
    if (dev->ops && dev->ops->close) {
        s32 ret = dev->ops->close(dev);
        if (ret != KERNEL_SUCCESS) return ret;
    }
    
    dev->enabled = false;
    printk("Driver: Device '%s' disabled\n", dev->name);
    return KERNEL_SUCCESS;
}

/* ============================================================================
 * INTERRUPT MANAGEMENT
 * =========================================================================== */

s32 irq_register(u32 irq, irq_handler_t handler, device_t *dev) {
    if (irq >= 256 || !handler) return KERNEL_EINVAL;
    
    if (irq_handlers[irq].handler) {
        return KERNEL_EBUSY;  /* IRQ already registered */
    }
    
    irq_handlers[irq].handler = handler;
    irq_handlers[irq].data = dev;
    irq_handlers[irq].device = dev;
    
    irq_enable(irq);
    printk("Driver: IRQ %d registered\n", irq);
    return KERNEL_SUCCESS;
}

s32 irq_unregister(u32 irq) {
    if (irq >= 256) return KERNEL_EINVAL;
    
    irq_disable(irq);
    irq_handlers[irq].handler = NULL;
    irq_handlers[irq].device = NULL;
    
    printk("Driver: IRQ %d unregistered\n", irq);
    return KERNEL_SUCCESS;
}

void irq_dispatch(u32 irq) {
    if (irq >= 256) return;
    
    if (irq_handlers[irq].handler) {
        irq_handlers[irq].handler(irq, irq_handlers[irq].data);
    }
    
    irq_ack(irq);
}

void irq_ack(u32 irq) {
    /* Send EOI (End Of Interrupt) to PIC */
    if (irq >= 8) {
        outb(0xA0, 0x20);  /* Slave PIC */
    }
    outb(0x20, 0x20);      /* Master PIC */
}

void irq_enable(u32 irq) {
    /* Enable in PIC */
    if (irq < 8) {
        u8 mask = inb(0x21);
        outb(0x21, mask & ~(1 << irq));
    } else {
        u8 mask = inb(0xA1);
        outb(0xA1, mask & ~(1 << (irq - 8)));
    }
}

void irq_disable(u32 irq) {
    /* Disable in PIC */
    if (irq < 8) {
        u8 mask = inb(0x21);
        outb(0x21, mask | (1 << irq));
    } else {
        u8 mask = inb(0xA1);
        outb(0xA1, mask | (1 << (irq - 8)));
    }
}
