# SimpleOS - Operating System Kernel Architecture

**Version:** 1.0.0  
**Language:** C with x86 Assembly  
**Target Platform:** x86 32-bit (Protected Mode)  
**Build System:** GNU Make + GCC

---

## Overview

SimpleOS is an educational operating system kernel demonstrating the core components of a modern OS. It implements the fundamental "central brain" that manages hardware resources and coordinates system activities.

### Key Objectives

1. **Resource Management** - CPU time, memory, I/O devices
2. **Process Scheduling** - Multitasking with priority-based scheduling
3. **Memory Management** - Virtual addressing, paging, physical allocation
4. **Device Management** - Unified driver framework for hardware devices
5. **File System** - Abstract VFS layer with built-in RAM filesystem

---

## Architecture Overview

### System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    SimpleOS Kernel                          │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │  Scheduler   │  │   Memory     │  │  File System │     │
│  │ (sched.c)    │  │  Manager     │  │  (vfs.c)     │     │
│  │              │  │  (mm.c)      │  │              │     │
│  │ -Tasks       │  │              │  │ -VFS abstraction   │
│  │ -Priority Q  │  │ -Paging      │  │ -Inode cache       │
│  │ -Preemption  │  │ -Buddy alloc │  │ -File descriptors  │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │           Device Driver Framework                    │  │
│  │                                                      │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐         │  │
│  │  │  PIC     │  │  PIT     │  │ Console  │ ...     │  │
│  │  │ (pic.c)  │  │ (pit.c)  │  │ Driver   │         │  │
│  │  │          │  │          │  │          │         │  │
│  │  │ -IRQ reg │  │ -Timers  │  │ -Output  │         │  │
│  │  │ -Masking │  │ -Ticks   │  │          │         │  │
│  │  └──────────┘  └──────────┘  └──────────┘         │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │        x86 Architecture Support (setup.c)           │   │
│  │  - GDT (Global Descriptor Table)                    │   │
│  │  - IDT (Interrupt Descriptor Table)                 │   │
│  │  - Paging (Virtual Memory)                          │   │
│  │  - CPU Feature Initialization                       │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                          │
                          │ Hardware Interrupts
                          │
┌─────────────────────────────────────────────────────────────┐
│              Hardware (CPU, Memory, Devices)                │
└─────────────────────────────────────────────────────────────┘
```

---

## Core Subsystems

### 1. **Process Scheduler** (`kernel/core/sched.c`)

**Responsibility:** Manages process execution, context switching, and CPU resource allocation

**Key Features:**
- **Task Control Block (TCB):** Data structure representing each process
  - Process ID (PID)
  - Program Counter & CPU state
  - Memory management info
  - File descriptor table
  - Parent/child relationships

- **Scheduling Algorithm:**
  - Priority-based (6 levels: IDLE, LOW, NORMAL, HIGH, REALTIME, and internal)
  - Round-robin time quantum within each priority
  - Preemption on timer tick

- **States:** CREATED → READY → RUNNING → BLOCKED → ZOMBIE

**API:**
```c
pid_t task_create(void (*entry)(void), sched_priority_t priority);
void task_exit(s32 status);
void sched_yield(void);
void sched_sleep(u32 ms);
task_t *sched_get_current(void);
s32 waitpid(pid_t pid, s32 *status);
```

### 2. **Memory Manager** (`kernel/mm/mm.c`)

**Responsibility:** Allocates and manages physical and virtual memory

**Key Features:**
- **Physical Allocation:** Buddy allocator system
  - Orders from 4KB (16 bytes) to 1MB
  - Efficient coalescing of free blocks
  - Fragmentation control

- **Virtual Addressing:**
  - 32-bit flat address space
  - 4KB page size
  - Hierarchical page tables (2-level: directory + table)

- **Page Flags:**
  - PRESENT: Page is in physical memory
  - WRITABLE: Writeable by processes
  - USER: Accessible from user mode
  - ACCESSED/DIRTY: For page replacement

**API:**
```c
void *kmalloc(u32 size);
void kfree(void *ptr);
void *kmalloc_aligned(u32 size, u32 alignment);
page_directory_t *vm_create_space(void);
s32 vm_map(page_directory_t *pgdir, u32 vaddr, u32 paddr, u32 flags);
s32 vm_unmap(page_directory_t *pgdir, u32 vaddr);
u32 vm_translate(page_directory_t *pgdir, u32 vaddr);
```

### 3. **Virtual File System** (`kernel/fs/vfs.c`)

**Responsibility:** Provides abstraction over different filesystems

**Key Features:**
- **VFS Abstraction:**
  - Inode: File metadata and operations
  - File: Open file with read/write offset
  - Directory: Container of directory entries

- **Operations:**
  - Read/write files
  - Create/delete files and directories
  - Directory traversal
  - Symbolic links support

- **File Descriptors:**
  - Per-process FD table (256 max per process)
  - Standard streams: stdin (0), stdout (1), stderr (2)

**API:**
```c
s32 vfs_open(const char *path, u32 flags, mode_t mode);
s32 vfs_close(fd_t fd);
s32 vfs_read(fd_t fd, void *buf, u32 n);
s32 vfs_write(fd_t fd, const void *buf, u32 n);
s32 vfs_mkdir(const char *path, mode_t mode);
s32 vfs_unlink(const char *path);
s32 vfs_stat(const char *path, inode_t *stat);
```

### 4. **Device Driver Framework** (`kernel/drivers/driver.c`)

**Responsibility:** Provides unified interface for managing hardware devices

**Key Features:**
- **Driver Registration:**
  - Probe and remove callbacks
  - Device lifecycle management
  - Hot-plug support

- **Device Types:**
  - STORAGE (disks, USB)
  - NETWORK (Ethernet, WiFi)
  - INPUT (keyboard, mouse)
  - OUTPUT (display, serial)
  - MISC (RTC, etc.)

- **Device Operations:**
  - read/write
  - ioctl (device control)
  - mmap (memory mapping)
  - interrupt handling

**API:**
```c
s32 driver_register(device_t *dev, driver_t *drv);
s32 driver_unregister(device_t *dev);
device_t *driver_lookup_device(const char *name);
s32 driver_register_irq(u8 irq, irq_handler_t handler);
s32 driver_unregister_irq(u8 irq);
```

### 5. **x86 Architecture Layer** (`kernel/arch/x86/`)

**Files:**
- `boot.S`: Bootloader - Real mode to Protected mode transition
- `setup.c`: GDT, IDT, paging initialization
- `irq.c`: IRQ and exception handling
- `exceptions.S`: Exception entry stubs
- `link.ld`: Linker script for memory layout

**Key Components:**

#### Global Descriptor Table (GDT)
- Defines memory segments for x86 protected mode
- 6 entries: NULL, kernel code, kernel data, user code, user data, TSS

#### Interrupt Descriptor Table (IDT)
- Maps 256 interrupts to handlers
- 0-31: CPU exceptions (divide by zero, page fault, etc.)
- 32-47: Hardware IRQs
- 128 (0x80): System call interface

#### Paging
- Maps virtual addresses to physical memory
- Supports demand paging and memory protection
- 4KB page size (standard for x86)

---

## Boot Process

### 1. **Bootloader** (GRUB or custom)
```
1. Real Mode Setup
   - Initialize segment registers
   - Check CPU (CPUID support)
   - Enable A20 line (20-bit address wrapping)

2. Load GDT
   - Define memory segments
   - lgdt instruction

3. Protected Mode Entry
   - Set CR0.PE (protection enable)
   - Long jump to protected mode code

4. Call kernel_main()
```

### 2. **Kernel Initialization** (kmain.c)
```
Phase 1: Memory Setup
   - Initialize buddy allocator
   - Set up kernel heap

Phase 2: Scheduler
   - Initialize task queues
   - Create idle task

Phase 3: File System
   - Initialize inode cache
   - Mount RAM filesystem

Phase 4: Driver Framework
   - Register driver table
   - Initialize device registry

Phase 5: System Setup
   - Create page directory
   - Enable virtual memory
   - Install exception handlers

Phase 6: First Task
   - Create init task (PID 1)
   - Launch scheduler
```

---

## File Structure

```
SimpleOS/
├── kernel/
│   ├── include/              # Header files (interfaces)
│   │   ├── kernel_types.h   # Fundamental types and macros
│   │   ├── sched.h          # Scheduler interface
│   │   ├── mm.h             # Memory manager interface
│   │   ├── fs.h             # File system interface
│   │   └── driver.h         # Driver framework interface
│   │
│   ├── core/                 # Core kernel subsystems
│   │   ├── kmain.c          # Kernel entry point
│   │   └── sched.c          # Task scheduler (400+ lines)
│   │
│   ├── mm/                   # Memory management
│   │   └── mm.c             # Paging and allocation (400+ lines)
│   │
│   ├── fs/                   # File system
│   │   ├── vfs.c            # Virtual file system (300+ lines)
│   │   └── ramfs.c          # RAM filesystem driver
│   │
│   ├── drivers/              # Device drivers
│   │   ├── driver.c         # Driver framework (350+ lines)
│   │   ├── pic.c            # 8259 PIC (interrupt controller)
│   │   └── pit.c            # 8254 PIT (timer)
│   │
│   └── arch/x86/            # x86-specific code
│       ├── boot.S           # Bootloader and mode switch
│       ├── setup.c          # GDT, IDT, paging setup
│       ├── irq.c            # IRQ/exception dispatcher
│       ├── exceptions.S     # Exception handlers
│       └── link.ld          # Linker script
│
├── Makefile                 # Build configuration
├── README.md                # This file
└── docs/
    ├── ARCHITECTURE.md      # Detailed architecture
    └── BUILDING.md          # Build instructions
```

---

## Building SimpleOS

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential binutils

# CentOS/RHEL
sudo yum groupinstall "Development Tools"

# macOS (requires cross-compiler)
brew install i386-elf-gcc
```

### Build Commands
```bash
# Build kernel
make

# Clean build artifacts
make clean

# Building options
make debug      # Debug symbols (-g -O0)
make release    # Optimized (-O3)

# Generate documentation
make docs
make disasm     # Disassemble kernel
make symbols    # List kernel symbols
make memmap     # Memory layout
```

### Run in Emulator
```bash
# QEMU (requires qemu-system-i386)
make qemu           # Boot kernel in emulator
make qemu-iso       # Boot from ISO image

# Bochs emulator (alternative)
bochs -f kernel.bxrc
```

---

## System Call Interface

**Future Implementation:**
- System calls via `int 0x80`
- Support for:
  - Process management (fork, exec, exit, wait)
  - File I/O (open, close, read, write)
  - Memory management (brk, mmap)
  - Signal handling
  - Socket operations (TCP/IP)

---

## Limitations & Future Work

### Current Limitations
- Single CPU (no multicore support yet)
- No real filesystem drivers (only RAM filesystem)
- No user/kernel privilege separation
- No preemptive scheduling yet (cooperative)
- Limited to x86 32-bit
- No network stack

### Future Enhancements
1. **Filesystems:**
   - FAT12/16/32 (floppy, USB)
   - ext2/ext3/ext4 (Linux-compatible)
   - ISO 9660 (CD-ROM)

2. **Devices:**
   - SATA/SCSI disk controllers
   - IDE/ATAPI
   - Serial port/USB drivers
   - Network interface cards
   - Input devices (keyboard, mouse)

3. **Advanced Scheduling:**
   - Preemptive multitasking
   - Real-time scheduling
   - Load balancing
   - Priority inheritance

4. **Memory:**
   - Demand paging
   - Page replacement algorithms
   - Shared memory regions
   - COW (Copy-on-Write)

5. **Architecture:**
   - x86-64 support
   - ARM support
   - RISC-V support

6. **Networking:**
   - TCP/IP stack
   - Socket interface
   - Network device drivers

---

## Learning Resources

### Relevant Documentation
- **Intel IA-32 Manual:** CPU instruction set and modes
- **OSDev.org:** Operating system development tutorials
- **Linux Kernel:** Reference implementation
- **xv6 OS:** Educational operating system (MIT)

### Key Concepts
1. **Protected Mode:** Segmentation, paging, privilege levels
2. **Virtual Memory:** Address translation, page tables, TLB
3. **Context Switching:** Task state, registers, stack management
4. **Interrupt Handling:** Hardware and software interrupts, handlers
5. **Synchronization:** Locks, semaphores, condition variables
6. **Filesystems:** Directory structure, inodes, file descriptors

---

## Debugging

### Using GDB
```bash
# Build with debug symbols
make debug

# Start QEMU in debug mode
qemu-system-i386 -kernel simpleos.elf -gdb tcp::1234 -S

# In another terminal:
gdb simpleos.elf
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
(gdb) info registers
(gdb) x /i $eip           # Examine instruction
```

### Serial Console Output
```bash
# Enable serial output in QEMU
qemu-system-i386 -kernel simpleos.elf -serial stdio
```

---

## Performance Characteristics

### Memory
- Kernel overhead: ~2MB (code + data)
- Page table: ~4KB per process per 4MB virtual space
- Scheduler overhead: O(1) for task selection

### CPU
- Context switch: ~1000 CPU cycles (estimate)
- System call overhead: ~500 cycles (int 0x80)
- Timer interrupt: ~1000 cycles (18.2 Hz or higher)

### Scalability
- Max processes: Limited by available RAM
- Max open files: 256 per process × max_processes
- Max inodes (RAM): ~1024 (configurable)

---

## License

This educational operating system is provided as a learning resource for understanding modern OS architecture and implementation.

---

**Last Updated:** 2024  
**Author:** SimpleOS Development Team  
**Status:** Educational Prototype - Not for production use
