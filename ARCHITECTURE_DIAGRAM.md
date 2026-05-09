# SimpleOS - System Architecture Diagram

## Complete Kernel Architecture

```
╔═══════════════════════════════════════════════════════════════════════════╗
║                        SIMPLEOS KERNEL STRUCTURE                          ║
╚═══════════════════════════════════════════════════════════════════════════╝

┌───────────────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER (Future)                         │
│                       init task, user processes                           │
└────────────────────────────────┬────────────────────────────────────────┘
                                 │ System calls / Interrupts
        ┌────────────────────────┴────────────────────────┐
        │                                                 │
┌───────▼──────────┐  ┌──────────────┐  ┌──────────────┐ │
│   Scheduler      │  │  Memory Mgmt │  │  File System │ │
│   (sched.c)      │  │   (mm.c)     │  │ (vfs/ramfs) │ │
│                  │  │              │  │              │ │
│ • 6 priority     │  │ • Buddy      │  │ • Inode-based│ │
│   queues         │  │   allocator  │  │ • RAMfs impl │ │
│ • Round-robin    │  │ • Page tables│  │ • VFS layer │ │
│ • Context switch │  │ • VM mapping │  │ • File ops  │ │
│ • Task states    │  │ • kmalloc()  │  │ • Dirs/files│ │
│                  │  │   kfree()    │  │              │ │
└───────┬──────────┘  └──────────────┘  └──────────────┘ │
        │              kernel/core/        kernel/mm/       kernel/fs/
        │                sched.c             mm.c            vfs.c
        │              sched.h              mm.h             ramfs.c
        │                                                     fs.h
        │
        └────────────────────────┬─────────────────────────┘
                                 │
        ┌────────────────────────▼─────────────────────────┐
        │  EXCEPTION & INTERRUPT HANDLING LAYER            │
        │          (exceptions.S, irq.c)                   │
        │                                                  │
        │ • CPU exceptions (divide-by-zero, page fault)  │
        │ • Hardware interrupt dispatch (PIČ routing)     │
        │ • Context switching on timer interrupt          │
        │ • ISR (Interrupt Service Routine) handlers      │
        └────────────────────────┬─────────────────────────┘
                                 │
        ┌────────────────────────▼─────────────────────────┐
        │        DEVICE DRIVER FRAMEWORK LAYER             │
        │            (drivers/ directory)                  │
        │                                                  │
        │  ┌─────────────┬─────────────┬──────────────┐   │
        │  │             │             │              │   │
        │  ▼             ▼             ▼              ▼   │
        │ ┌──┐          ┌──┐         ┌──┐          ┌──┐  │
        │ │PIC          │PIT         │IO            │xx   │
        │ │Interrupt    │Timer       │Console       │New  │
        │ │Controller   │(18.2 Hz)   │(Serial)      │Dev  │
        │ │(8259)       │(8254)      │             │    │
        │ └──┐          └──┐         └──┐          └──┐  │
        │    └─────────────┴────────────┴───────────┘   │
        │                                               │
        │    driver_register() | driver_unregister()    │
        │    handle_irq() callbacks                     │
        └────────────────────────┬──────────────────────┘
                                 │
        ┌────────────────────────▼──────────────────────┐
        │   x86 ARCHITECTURE LAYER (arch/x86/)          │
        │                                               │
        │ ┌─────────────────────────────────────────┐  │
        │ │ GDT (Global Descriptor Table)           │  │
        │ │ • Kernel code segment                   │  │
        │ │ • Kernel data segment                   │  │
        │ │ • TSS (Task State Segment)              │  │
        │ └─────────────────────────────────────────┘  │
        │                                               │
        │ ┌─────────────────────────────────────────┐  │
        │ │ IDT (Interrupt Descriptor Table)        │  │
        │ │ • Exception handlers (0-31)             │  │
        │ │ • Hardware IRQ handlers (32-47)         │  │
        │ │ • Syscall gate (0x80)                   │  │
        │ └─────────────────────────────────────────┘  │
        │                                               │
        │ ┌─────────────────────────────────────────┐  │
        │ │ Paging & Virtual Memory                 │  │
        │ │ • Page directory (4 MB mapped)          │  │
        │ │ • Page tables (4 KB pages)              │  │
        │ │ • Page fault handler                    │  │
        │ └─────────────────────────────────────────┘  │
        │                                               │
        │ ┌─────────────────────────────────────────┐  │
        │ │ Exception Handlers (exceptions.S)       │  │
        │ │ • Divide error, Debug, NMI             │  │
        │ │ • Breakpoint, Overflow, Bounds         │  │
        │ │ • Invalid opcode, Device not available│  │
        │ │ • Page fault, General protection      │  │
        │ └─────────────────────────────────────────┘  │
        │                                               │
        │          setup.c | irq.c | link.ld           │
        └────────────────────────┬──────────────────────┘
                                 │
        ┌────────────────────────▼──────────────────────┐
        │                                               │
        │     ┌──────────────────────────────────┐     │
        │     │       BOOTLOADER (boot.S)        │     │
        │     │                                  │     │
        │     │ Real Mode (16-bit)               │     │
        │     │ • CPU detection                  │     │
        │     │ • A20 line enable                │     │
        │     │                                  │     │
        │     │ Protected Mode (32-bit)          │     │
        │     │ • Load GDT                       │     │
        │     │ • Switch to protected mode       │     │
        │     │ • Jump to kernel_main()          │     │
        │     └──────────────────────────────────┘     │
        │                                               │
        │   Boot → Multiboot → kernel_main (kmain.c)   │
        └────────────────────────┬──────────────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │    x86 CPU Hardware     │
                    │  • 32-bit protected     │
                    │  • Paging enabled       │
                    │  • Interrupts active    │
                    └─────────────────────────┘
```

## Data Flow: Timer Interrupt Example

```
1. Hardware timer (PIT at 18.2 Hz) fires
   └─> IRQ0 signal to CPU

2. CPU stops current task, saves context
   └─> Calls IDT entry 32 (IRQ0 handler)

3. kernel/arch/x86/irq.c: handle_irq(32)
   └─> Calls registered PIT driver handler

4. kernel/drivers/pit.c: pit_handle_irq()
   └─> Increments tick counter
   └─> Signals scheduler to switch tasks

5. kernel/core/sched.c: scheduler_context_switch()
   └─> Selects next task from ready queue
   └─> Saves current task context
   └─> Loads new task context

6. CPU resumes execution with new task
   └─> New task runs until next timer interrupt
```

## Memory Layout

```
0x00000000  ┌─────────────────────────┐
            │ Real Mode IVT & BIOS    │ (1 KB)
            │ (interrupt vectors)     │
0x00001000  ├─────────────────────────┤
            │ Boot code               │ (6 KB)
            │ (loaded by GRUB)        │
0x00100000  ├─────────────────────────┤
            │ Kernel .text (code)     │ Mapped by paging
            │ Kernel .data            │ (identity mapped)
            │ Kernel .bss             │ (used by kernel)
0x01000000  ├─────────────────────────┤
            │ Kernel heap             │ (grows up)
            │ (allocated by kmalloc)  │
            │                         │
0x7F000000  ├─────────────────────────┤
            │ Kernel stack            │ (grows down)
            │                         │
0x80000000  ├─────────────────────────┤
            │ Page tables             │ (not shown)
            │ (allocated separately)  │
            └─────────────────────────┘
            
Total configured: 128 MB (adjustable via mm_init)
```

## Initialization Sequence

```
Start: CPU in protected mode (set by bootloader)
  │
  ├─> kernel_main() called from boot.S
  │
  ├─> Phase 1: Memory Setup
  │   └─> mm_init(128 MB)
  │       • Buddy allocator init
  │       • Free block list setup
  │
  ├─> Phase 2: Scheduler Setup
  │   └─> scheduler_init()
  │       • Create 6 priority queues
  │       • Initialize TCB pool
  │
  ├─> Phase 3: File System Setup
  │   └─> vfs_init()
  │   └─> ramfs_register()
  │       • Create root inode
  │       • Mount file system
  │
  ├─> Phase 4: Driver Setup
  │   └─> driver_init()
  │   └─> pic_init()
  │       • Initialize interrupt controller
  │   └─> pit_init()
  │       • Initialize timer (18.2 Hz)
  │
  ├─> Phase 5: Virtual Memory Activation
  │   └─> vm_create_space()
  │   └─> vm_activate()
  │       • Enable paging (CR0 |= 0x80000000)
  │       • Use page tables
  │
  ├─> Phase 6: Start Init Task
  │   └─> task_create("init", ...)
  │   └─> scheduler_add_task(init_task)
  │
  └─> scheduler_run()
      │
      ├─> Enable interrupts (STI)
      │
      └─> Infinite idle loop
          └─> Wait for timer interrupts
              └─> Each tick calls scheduler_context_switch()
```

## File Organization

```
kernel/
├── include/
│   ├── kernel_types.h      ← Type defs (u32, KERNEL_STATUS, etc.)
│   ├── sched.h             ← Scheduler API
│   ├── mm.h                ← Memory manager API
│   ├── fs.h                ← File system API
│   ├── driver.h            ← Driver framework API
│   ├── list.h              ← Intrusive linked list
│   └── ... (other headers)
│
├── core/
│   ├── kmain.c             ← Kernel initialization (6 phases)
│   ├── sched.c             ← Task scheduling, context switch
│   └── Makefile (included)
│
├── mm/
│   ├── mm.c                ← Buddy allocator + paging
│   └── Makefile
│
├── fs/
│   ├── vfs.c               ← Virtual file system abstraction
│   ├── ramfs.c             ← RAM filesystem implementation
│   └── Makefile
│
├── drivers/
│   ├── driver.c            ← Driver registration framework
│   ├── pic.c               ← Programmable Interrupt Controller
│   ├── pit.c               ← Programmable Interval Timer
│   ├── console.c           ← Serial console
│   └── Makefile
│
└── arch/
    └── x86/
        ├── boot.S          ← Bootloader, real→protected mode
        ├── setup.c         ← GDT, IDT, paging init
        ├── irq.c           ← Interrupt dispatcher
        ├── exceptions.S    ← Exception handlers
        ├── link.ld         ← Linker script (memory layout)
        └── grub.cfg        ← GRUB bootloader config
```

## How It All Connects 🔗

1. **Boot:** boot.S → setup.c (GDT, IDT) → kernel_main()
2. **Initialization:** kernel_main() calls each subsystem's init in order
3. **Runtime:** Timer interrupts → IRQ handler → scheduler switch
4. **Subsystems:** Talk through clean APIs (kmalloc, vfs_open, driver_register)
5. **I/O:** All goes through printk → serial console (0x3F8)

## Adding New Code

**New Driver:**
- Create kernel/drivers/mydev.c
- Implement init and IRQ handler
- Call driver_register() in kmain.c
- Update kernel/drivers/Makefile

**New Syscall:**
- Add to sched.h, implement in sched.c
- Call from init task
- Add dispatch in kmain.c if needed

**New Filesystem:**
- Implement ops struct (vfs.c interface)
- Call vfs_register_fs() in kmain.c
- Similar structure to ramfs.c

---

**That's the complete architecture!** Every line of kernel code fits into this structure.
