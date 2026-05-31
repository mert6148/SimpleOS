# SimpleOS Educational x86 Kernel

This kernel is a small educational operating system designed to boot in an x86 emulator and demonstrate the main subsystems of a real OS.

## Build and Run

Requirements:
- `gcc` with 32-bit target support (`-m32`) or `i686-elf-gcc`
- `qemu-system-i386`
- `make`

Build the kernel:

```bash
cd kernel
make
```

Run it in QEMU:

```bash
cd kernel
make run
```

If your toolchain is not 32-bit capable, set the compiler explicitly:

```bash
make CC=i686-elf-gcc
```

## Kernel Architecture

### Boot Flow

1. `arch/x86/boot.S` runs in 16-bit real mode.
2. It performs CPU checks, enables the A20 line, and loads the Global Descriptor Table (GDT).
3. It switches the CPU into protected mode and jumps to `kernel_main()`.
4. The kernel begins its core initialization and prints startup messages to the serial console.

### Core Kernel Initialization

The kernel entrypoint is `core/kmain.c`.

- `mm_init()` initializes the memory manager and prints memory statistics.
- `sched_init()` initializes the scheduler and ready queues.
- `vfs_init()` initializes the virtual file system.
- `driver_init()` initializes the driver framework.
- `vm_create_space()` and `vm_activate()` enable paging.
- `task_create()` creates the first init task.
- `sched_yield()` begins the scheduler loop.

### Process Scheduler

Implemented in `core/sched.c`.

- Priority-based round-robin scheduling with 5 levels.
- `task_create()` allocates a task control block (TCB) and places tasks in run queues.
- `sched_tick()` is intended to be called by a timer interrupt for preemption.
- `sched_yield()` selects the next ready task.

### Memory Management

Implemented in `mm/mm.c`.

- A buddy-style allocator tracks free pages.
- `kmalloc()` allocates kernel memory.
- `vm_create_space()` creates a new page directory and identity maps the first 4MB.
- `vm_map_page()` can map virtual pages to physical pages.
- Paging is enabled in `arch/x86/setup.c`.

### Virtual File System

Implemented in `fs/vfs.c`.

- `vfs_init()` sets up in-memory file descriptor tables.
- `vfs_open()`, `vfs_read()`, `vfs_write()`, and `vfs_close()` provide a basic file API.
- This is a RAM-backed filesystem model for educational use.

### Interrupts and Driver Framework

- `arch/x86/exceptions.S` defines CPU exception and IRQ stubs.
- `arch/x86/setup.c` installs the IDT and enables protected-mode interrupts.
- `drivers/driver.c` implements a generic driver registration and IRQ dispatch model.
- `drivers/pic.c` programs the 8259 PIC.
- `drivers/pit.c` programs the PIT timer for periodic interrupts.

## Notes

This kernel is intentionally small and educational. Many subsystems are simplified to make the design easy to follow.

- The scheduler is a basic proof-of-concept with task metadata and ready queues.
- The memory manager shows paging and page allocation, but some full allocator features are simplified.
- The filesystem is a RAM-backed VFS that illustrates file descriptors and inode metadata.
- The driver framework is designed to show device registration, interrupt handling, and hardware abstraction.
