/**
 * SimpleOS Programmable Interval Timer Driver
 * Provides system clock and scheduling ticks for x86 systems
 */

#include "kernel_types.h"
#include "driver.h"

/* PIT I/O Ports */
#define PIT_CHANNEL0   0x40    /* Counter 0 (1MHz clock, system timer) */
#define PIT_CHANNEL1   0x41    /* Counter 1 (not typically used) */
#define PIT_CHANNEL2   0x42    /* Counter 2 (speaker/tone generation) */
#define PIT_COMMAND    0x43    /* Control word register */

#define PIT_BASE_FREQ  1193182  /* 1.193182 MHz base clock */

/* Default tick rate: 100 Hz (10ms ticks) */
static u32 pit_frequency = 100;
static u32 pit_divisor;

/**
 * Initialize Programmable Interval Timer
 * Sets up channel 0 for periodic interrupt generation
 */
s32 pit_init(void) {
    /* Calculate divisor for desired frequency */
    pit_divisor = PIT_BASE_FREQ / pit_frequency;
    
    if (pit_divisor > 0xFFFF) {
        /* Frequency too low */
        pit_divisor = 0xFFFF;
        pit_frequency = PIT_BASE_FREQ / pit_divisor;
    }
    
    /* Send control word:
       Bits 7-6: Channel select (00 = channel 0)
       Bits 5-4: Access mode (11 = low byte then high byte)
       Bits 3-1: Operating mode (010 = rate generator)
       Bit 0:    BCD mode (0 = binary)
    */
    u8 control = 0x34;  /* 0011 0100 */
    
    outb(PIT_COMMAND, control);
    
    /* Load divisor (low byte first, then high byte) */
    outb(PIT_CHANNEL0, (u8)(pit_divisor & 0xFF));
    outb(PIT_CHANNEL0, (u8)((pit_divisor >> 8) & 0xFF));
    
    return KERNEL_SUCCESS;
}

/**
 * Set PIT frequency
 * Frequency must be between 18 Hz (slowest) and 1.19 MHz (fastest)
 */
s32 pit_set_frequency(u32 freq) {
    if (freq < 18 || freq > PIT_BASE_FREQ) {
        return KERNEL_EINVAL;
    }
    
    pit_frequency = freq;
    pit_divisor = PIT_BASE_FREQ / freq;
    
    /* Reprogram timer */
    u8 control = 0x34;
    outb(PIT_COMMAND, control);
    outb(PIT_CHANNEL0, (u8)(pit_divisor & 0xFF));
    outb(PIT_CHANNEL0, (u8)((pit_divisor >> 8) & 0xFF));
    
    return KERNEL_SUCCESS;
}

/**
 * Get current PIT frequency
 */
u32 pit_get_frequency(void) {
    return pit_frequency;
}

/**
 * Get current PIT divisor
 */
u32 pit_get_divisor(void) {
    return pit_divisor;
}

/**
 * Read counter value (running count)
 * Note: This might lose some ticks due to race conditions
 */
u16 pit_read_counter(void) {
    /* Latch current counter value */
    outb(PIT_COMMAND, 0x00);  /* Latch command */
    
    u16 count = inb(PIT_CHANNEL0);
    count |= (u16)inb(PIT_CHANNEL0) << 8;
    
    return count;
}

/**
 * Microsecond precision delay using PIT
 * Blocks CPU until specified microseconds have passed
 */
void pit_udelay(u32 microseconds) {
    if (microseconds == 0) {
        return;
    }
    
    /* This is a busy-wait loop - not ideal but works */
    /* In a real OS, you'd use interrupts and sleep queue */
    
    u32 ticks_per_second = pit_frequency;
    u32 ticks_needed = (microseconds * ticks_per_second) / 1000000;
    
    /* Reset counter to max to get predictable timing */
    outb(PIT_COMMAND, 0x30);
    outb(PIT_CHANNEL0, 0xFF);
    outb(PIT_CHANNEL0, 0xFF);
    
    /* Wait for counter to count down */
    u32 elapsed = 0;
    while (elapsed < ticks_needed) {
        u16 cnt = pit_read_counter();
        elapsed = (0xFFFF - cnt) * 1000000 / PIT_BASE_FREQ;
    }
    
    /* Restore normal timer */
    pit_init();
}

/**
 * Millisecond precision delay
 */
void pit_mdelay(u32 milliseconds) {
    pit_udelay(milliseconds * 1000);
}

/**
 * Timer test - measure actual frequency
 */
static u32 pit_test_ticks = 0;

void pit_test_handler(void) {
    pit_test_ticks++;
}

u32 pit_calibrate(void) {
    /* This would measure actual system clock frequency */
    /* Not implemented - would need external reference like RTC */
    return PIT_BASE_FREQ / pit_divisor;
}
