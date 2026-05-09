# SimpleOS - Quick Start & What You'll See

## TL;DR - Get Running in 2 Minutes

```bash
# Windows: Install WSL2, then:
wsl
cd /mnt/c/Users/mertd/OneDrive/Masaüstü/SimpleOS
make qemu

# Or use PowerShell:
.\build.ps1 qemu
```

**You'll see:**
```
╔════════════════════════════════════════════════════════════╗
║         SimpleOS Kernel v1.0.0 - Starting...              ║
║         Building the Central Brain...                     ║
╚════════════════════════════════════════════════════════════╝

[KERNEL] 🔧 Phase 1: Setting up memory management...
[MM] Initializing buddy allocator (128 MB)...
[MM] ✓ Memory manager ready

[KERNEL] 🔧 Phase 2: Initializing scheduler...
[SCHED] ✓ Scheduler ready (6 priority levels)

[KERNEL] 🔧 Phase 3: Setting up file systems...
[VFS] ✓ VFS initialized
[RAMFS] ✓ RAM filesystem registered

[KERNEL] 🔧 Phase 4: Initializing device drivers...
[PIC] ✓ Programmable Interrupt Controller ready
[PIT] ✓ Programmable Interval Timer (18.2 Hz) ready

[KERNEL] 🔧 Phase 5: Activating virtual memory (paging)...
[VM] ✓ Virtual memory activated
[PAGING] ✓ Paging enabled (4KB pages)

[KERNEL] 🔧 Phase 6: Starting first user task...
[KERNEL] ✓ SimpleOS fully booted and ready

[INIT] Running init task...
```

**Exit with:** `Ctrl+A` then `X`

---

## What Just Happened? 🏗️

SimpleOS just performed a **real operating system boot sequence**:

1. **Bootloader** (x86 assembly) - Transitioned from real mode to protected mode
2. **Kernel Entry** (kmain.c) - Started initializing kernel subsystems
3. **Memory Manager** - Set up buddy allocator for dynamic memory
4. **Scheduler** - Prepared 6-level priority task queues
5. **File System** - Mounted RAM-backed virtual file system
6. **Drivers** - Initialized hardware (PIC interrupt controller, PIT timer)
7. **Paging** - Enabled virtual memory with 4KB pages
8. **Init Task** - Created first user process (pid=1)

**All real kernel code**, not a simulation. The kernel is now running and idle, waiting for interrupts.

---

## Architecture Overview 🧠

```
┌─────────────────────────────────────────────────────────────┐
│                   SIMPLEOS KERNEL                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  Scheduler   │  │   MM (Paging)│  │  File System │      │
│  │  (6-level    │  │  + Buddy     │  │  (VFS+RAMfs)│      │
│  │   priority)  │  │   allocator) │  │              │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         ▲                  ▲                  ▲              │
│         │                  │                  │              │
│  ┌────────────────────────────────────────────────────┐     │
│  │         Interrupt & Exception Handling             │     │
│  │    (IDT, exceptions.S, hardware IRQs)              │     │
│  └────────────────────────────────────────────────────┘     │
│                          ▲                                   │
│  ┌────────────────────────────────────────────────────┐     │
│  │              Device Driver Framework                │     │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐         │     │
│  │  │  PIC     │  │  PIT     │  │ Console  │         │     │
│  │  │(Interrupt)  │(Timer)    │  │(Serial)  │         │     │
│  │  └──────────┘  └──────────┘  └──────────┘         │     │
│  └────────────────────────────────────────────────────┘     │
│                          ▲                                   │
│  ┌────────────────────────────────────────────────────┐     │
│  │         x86 Architecture Layer                     │     │
│  │  GDT | IDT | Paging | Context Switching           │     │
│  │  Boot (real→protected mode) | Exceptions          │     │
│  └────────────────────────────────────────────────────┘     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                          │
                          ▼
                  CPU Hardware (x86)
```

---

## File Structure 📁

```
SimpleOS/
├── kernel/                          ← Everything kernel-related
│   ├── arch/x86/
│   │   ├── boot.S                   ← Real mode → Protected mode
│   │   ├── setup.c                  ← GDT, IDT, paging init
│   │   ├── exceptions.S             ← Exception handlers
│   │   ├── irq.c                    ← Interrupt dispatch
│   │   └── link.ld                  ← Memory layout
│   │
│   ├── core/
│   │   ├── kmain.c                  ← Kernel initialization
│   │   └── sched.c                  ← Process scheduler
│   │
│   ├── mm/
│   │   └── mm.c                     ← Memory manager (buddy + paging)
│   │
│   ├── fs/
│   │   ├── vfs.c                    ← Virtual File System
│   │   └── ramfs.c                  ← RAM-backed filesystem
│   │
│   ├── drivers/
│   │   ├── driver.c                 ← Driver framework
│   │   ├── pic.c                    ← Interrupt controller
│   │   └── pit.c                    ← Timer
│   │
│   └── include/
│       ├── kernel_types.h           ← Type definitions
│       ├── sched.h                  ← Scheduler API
│       ├── mm.h                     ← Memory manager API
│       ├── fs.h                     ← File system API
│       └── driver.h                 ← Driver API
│
├── docs/                            ← HTML documentation
├── Makefile                         ← Build configuration
├── README.md                        ← Learning guide
├── ARCHITECTURE.md                  ← Subsystem design details
├── BUILDING.md                      ← Build procedures
└── SUMMARY.md                       ← File reference

Build artifacts (after `make`):
├── simpleos.elf                     ← Kernel executable (bootable)
├── *.o                              ← Compiled object files
└── simpleos.iso                     ← Bootable ISO (if `make iso`)
```

---

## Key Subsystems Explained 🔧

### 1️⃣ Process Scheduler (`kernel/core/sched.c`)
- **What it does:** Manages task/process creation and execution
- **Features:** 
  - 6 priority levels (IDLE, LOW, NORMAL, HIGH, REALTIME)
  - Round-robin within each priority (20ms time slices)
  - Pre-emptive multitasking
  - Task states: CREATED → READY → RUNNING → BLOCKED → ZOMBIE
- **You can:** Create processes, assign priorities, block/wake tasks

### 2️⃣ Memory Manager (`kernel/mm/mm.c`)
- **Physical memory:** Buddy allocator (free blocks coalesce efficiently)
- **Virtual memory:** 4KB paging with 2-level page tables
- **Allocation:** `kmalloc()` (dynamic) and `vm_map()` (virtual address mapping)
- **Deallocation:** `kfree()` and `vm_unmap()`
- **128 MB** configured at boot

### 3️⃣ Virtual File System (`kernel/fs/vfs.c` + `kernel/fs/ramfs.c`)
- **Abstraction:** Mount different filesystems (currently RAMfs)
- **Operations:** open(), read(), write(), close(), seek()
- **RAMfs:** All files stored in RAM (no disk I/O)
- **Future:** Can add ext2, FAT, etc. by implementing same interface

### 4️⃣ Driver Framework (`kernel/drivers/driver.c`)
- **Unified interface:** Register drivers with IRQ handlers
- **Implemented:**
  - **PIC** (8259): Programmable Interrupt Controller
  - **PIT** (8254): Timer generating 18.2 Hz interrupts
  - **Console:** Serial output
- **Extensible:** Add new drivers by implementing handler functions

### 5️⃣ x86 Boot & Architecture (`kernel/arch/x86/`)
- **boot.S:** Real mode assembly → protected mode transition
- **setup.c:** GDT (Global Descriptor Table), IDT (Interrupt Descriptor Table)
- **Paging:** Virtual memory translation with page fault handling
- **exceptions.S:** CPU exception handlers (divide-by-zero, page fault, etc.)

---

## Code Patterns & Conventions 📝

### Types (from `kernel_types.h`)
```c
u8, u16, u32, u64      // unsigned integers
s8, s16, s32, s64      // signed integers
bool                    // boolean (True/False)
KERNEL_STATUS           // return code type
```

### Return Codes
```c
KERNEL_SUCCESS      // 0 - operation succeeded
KERNEL_ENOMEM      // -4 - out of memory
KERNEL_EINVAL      // -7 - invalid argument
// Full list in kernel/include/kernel_types.h
```

### Output
```c
#include "kernel_types.h"

// Use printk() for all kernel logging
// Output goes to serial console (COM1 at 0x3F8)
printk("Task %d scheduled\n", pid);
```

### Memory
```c
void *ptr;
KERNEL_STATUS status = kmalloc(1024, &ptr);
if (status != KERNEL_SUCCESS) {
    // Handle error
    return status;
}
// Use ptr...
kfree(ptr);
```

---

## Modifying the Kernel 🔨

### Add a New Syscall
1. Define in `kernel/include/sched.h`
2. Implement in `kernel/core/sched.c`
3. Add to syscall dispatch table
4. Test with `make qemu`

### Add a New Driver
1. Create `kernel/drivers/mydevice.c`
2. Implement IRQ handler and init function
3. Call `driver_register("mydevice", IRQ_NUM, &ops)`
4. Update `kernel/core/kmain.c` initialization
5. Rebuild: `make && make qemu`

### Debug
```bash
# Build with debug symbols
CFLAGS="-g" make

# Run in QEMU with gdb
qemu-system-i386 -kernel simpleos.elf -serial stdio -s -S

# In another terminal
i386-elf-gdb simpleos.elf
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

---

## Learning Path 🚀

1. **Start here:** Read `README.md` (learning guide)
2. **Architecture:** Review `ARCHITECTURE.md` (design decisions)
3. **Code exploration:**
   - `kernel/core/kmain.c` - initialization sequence
   - `kernel/core/sched.c` - process management
   - `kernel/mm/mm.c` - memory management
   - `kernel/arch/x86/boot.S` - boot process
4. **Extend:** Try adding a new driver or syscall
5. **Debug:** Use `printk()` liberally and watch serial output

---

## Troubleshooting 🐛

| Problem | Solution |
|---------|----------|
| Build fails | Check BUILDING.md, install gcc/make |
| QEMU not found | `sudo apt install qemu-system-i386` (on Linux/WSL) |
| Kernel exits immediately | Check serial output for panic messages in `make qemu` |
| Page fault | Memory manager bug - add `printk()` debug statements |
| Task not scheduling | Check sched.c - verify task was added to ready queue |

---

## Next Steps 🎯

- **Modify boot message:** Edit `kernel/core/kmain.c` line ~50
- **Add a new driver:** See `kernel/drivers/pit.c` as template
- **Create a syscall:** Extend `kernel/core/sched.c` and call from init task
- **Change scheduler:** Modify `sched.c` algorithms
- **Add file system:** Implement ops in `kernel/fs/` directory

---

**Ready? Run:** `make qemu`

Exit QEMU: `Ctrl+A` then `X`
