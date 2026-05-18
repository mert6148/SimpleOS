/**
 * SimpleOS Project Configuration
 * Build, compiler, and development settings
 */

#ifndef SIMPLEOS_CONFIG_H
#define SIMPLEOS_CONFIG_H

/* ============================================================================
 * BUILD CONFIGURATION
 * =========================================================================== */

/** Enable debug symbols and logging */
#define CONFIG_DEBUG 1

/** Verbose kernel output */
#define CONFIG_VERBOSE 0

/** Log level: 0=silent, 1=error, 2=warn, 3=info, 4=debug */
#define CONFIG_LOG_LEVEL 2

/* ============================================================================
 * KERNEL CONFIGURATION
 * =========================================================================== */

/** Maximum number of concurrent tasks */
#define CONFIG_MAX_TASKS 32

/** Scheduler priority levels */
#define CONFIG_PRIORITY_LEVELS 6

/** Stack size per task (bytes) */
#define CONFIG_TASK_STACK_SIZE 4096

/** Total heap memory (bytes) - 256 MB */
#define CONFIG_KERNEL_HEAP_SIZE (256 * 1024 * 1024)

/** Memory page size (bytes) */
#define CONFIG_PAGE_SIZE 4096

/** Enable virtual memory/paging */
#define CONFIG_ENABLE_PAGING 1

/** Enable process isolation */
#define CONFIG_ENABLE_ISOLATION 1

/* ============================================================================
 * SCHEDULER CONFIGURATION
 * =========================================================================== */

/** Time slice per task (milliseconds) */
#define CONFIG_TIME_SLICE_MS 10

/** Enable round-robin scheduling */
#define CONFIG_ROUND_ROBIN 1

/** Enable priority preemption */
#define CONFIG_PREEMPTION 1

/* ============================================================================
 * FILESYSTEM CONFIGURATION
 * =========================================================================== */

/** Maximum open files per process */
#define CONFIG_MAX_OPEN_FILES 64

/** Maximum file size (bytes) - 100 MB */
#define CONFIG_MAX_FILE_SIZE (100 * 1024 * 1024)

/** Enable VFS (Virtual File System) */
#define CONFIG_ENABLE_VFS 1

/** Enable RAM filesystem */
#define CONFIG_ENABLE_RAMFS 1

/** RAM filesystem size (bytes) - 50 MB */
#define CONFIG_RAMFS_SIZE (50 * 1024 * 1024)

/* ============================================================================
 * DEVICE CONFIGURATION
 * =========================================================================== */

/** Enable PIC (Programmable Interrupt Controller) */
#define CONFIG_ENABLE_PIC 1

/** Enable PIT (Programmable Interval Timer) */
#define CONFIG_ENABLE_PIT 1

/** Enable serial console */
#define CONFIG_ENABLE_SERIAL 1

/** Serial baud rate */
#define CONFIG_SERIAL_BAUD_RATE 9600

/** Serial port I/O base address (x86) */
#define CONFIG_SERIAL_PORT_BASE 0x3F8

/* ============================================================================
 * SAFETY & SECURITY
 * =========================================================================== */

/** Enable stack bounds checking */
#define CONFIG_STACK_CHECK 1

/** Enable memory protection */
#define CONFIG_MEMORY_PROTECTION 1

/** Enable system call validation */
#define CONFIG_VALIDATE_SYSCALLS 1

/** Panic on kernel errors (vs. graceful shutdown) */
#define CONFIG_PANIC_ON_ERROR 1

/* ============================================================================
 * PERFORMANCE
 * =========================================================================== */

/** Enable profiling counters */
#define CONFIG_ENABLE_PROFILING 0

/** Enable performance monitoring */
#define CONFIG_ENABLE_PMC 0

/** Cache buddy allocator results */
#define CONFIG_CACHE_ALLOCATOR 1

/* ============================================================================
 * DEBUGGING
 * =========================================================================== */

/** Enable kernel assert macros */
#define CONFIG_ENABLE_ASSERTS 1

/** Enable detailed kernel traces */
#define CONFIG_ENABLE_TRACES 0

/** Maximum trace entries */
#define CONFIG_MAX_TRACES 1024

/** Enable debugger support */
#define CONFIG_ENABLE_DEBUGGER 0

/* ============================================================================
 * PLATFORM SPECIFIC
 * =========================================================================== */

/** Target architecture: x86, x64, arm */
#define CONFIG_ARCH "x86"

/** Target bits: 32 or 64 */
#define CONFIG_BITS 32

/** Enable x86 protected mode */
#define CONFIG_X86_PROTECTED_MODE 1

/** Enable x86 paging */
#define CONFIG_X86_PAGING 1

/** Enable x86 interrupts */
#define CONFIG_X86_INTERRUPTS 1

/* ============================================================================
 * OPTIONAL FEATURES
 * =========================================================================== */

/** Enable network stack (if applicable) */
#define CONFIG_ENABLE_NETWORKING 0

/** Enable graphics subsystem */
#define CONFIG_ENABLE_GRAPHICS 0

/** Enable USB support */
#define CONFIG_ENABLE_USB 0

/** Enable ACPI support */
#define CONFIG_ENABLE_ACPI 0

#endif /* SIMPLEOS_CONFIG_H */
