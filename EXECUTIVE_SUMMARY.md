# SimpleOS - Executive Summary

**SimpleOS** is a **complete, bootable x86 operating system kernel** written from scratch in C and assembly. It's not a toy or simulation—it's real kernel code that boots and runs.

## Quick Facts

| Aspect | Details |
|--------|---------|
| **Language** | C + x86 Assembly (32-bit protected mode) |
| **Size** | ~3,450 lines of kernel code |
| **Status** | ✅ Complete & bootable |
| **Platform** | x86 architecture, runs on QEMU |
| **Build** | Single `make` command |
| **Purpose** | Educational OS learning |

## What It Includes ✅

### 1. **Boot Process** ⚡
- Real mode → Protected mode transition (boot.S)
- GDT & IDT setup (setup.c)
- Multiboot loader support
- Proper memory layout and segments

### 2. **Process Scheduler** 🔄
- Priority-based with 6 levels
- Round-robin time slicing (20ms)
- Pre-emptive multitasking
- Task creation, context switching, blocking

### 3. **Memory Management** 💾
- **Physical:** Buddy allocator (16 bytes → 1 MB blocks)
- **Virtual:** 4KB paging with 2-level page tables
- Dynamic allocation with `kmalloc()` / `kfree()`
- Virtual address mapping with `vm_map()` / `vm_unmap()`

### 4. **Virtual File System** 📁
- VFS abstraction layer
- Inode-based model
- File operations: open, read, write, close, seek
- RAMfs implementation (files in RAM)
- Extensible for future filesystems

### 5. **Interrupt Handling** 🛡️
- Full exception handler setup (divide-by-zero, page fault, etc.)
- Hardware interrupt dispatch
- IRQ routing to drivers

### 6. **Driver Framework** 🔌
- Unified driver registration API
- **PIC (8259):** Programmable Interrupt Controller
- **PIT (8254):** 18.2 Hz system timer
- **Console:** Serial output
- Ready for new drivers

### 7. **Documentation** 📚
- README.md - Learning guide
- ARCHITECTURE.md - Design details
- BUILDING.md - Build procedures
- SUMMARY.md - File reference
- HTML docs with interactive examples

## Boot Sequence 🚀

When you run `make qemu`:

```
1. Bootloader (boot.S) starts in real mode
   ↓
2. CPU mode switched to protected mode
   ↓
3. kernel_main(kmain.c) called
   ↓
4. Memory manager initialized (128 MB)
   ↓
5. Scheduler setup (6 priority queues)
   ↓
6. File system mounted (RAMfs)
   ↓
7. Device drivers registered (PIC, PIT)
   ↓
8. Virtual memory activated (paging enabled)
   ↓
9. First user task created (init process)
   ↓
10. Kernel idle (waiting for timer interrupts)
```

**Visible output:** Clear boot messages showing each phase with ✓ status indicators.

## Architecture Layers 🏗️

```
┌─────────────────────────────────────────┐
│        User Application Layer           │  (Future)
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│    Scheduler | MM | VFS | Drivers       │  (Kernel)
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│     Exception Handling & Interrupts     │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│  x86 Architecture: GDT, IDT, Paging    │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│          x86 CPU Hardware               │
└─────────────────────────────────────────┘
```

## Code Quality 📊

✅ **Well-organized:** Clear subsystem separation  
✅ **Documented:** Every major function commented  
✅ **Extensible:** Plugin architecture for drivers  
✅ **Educational:** Code written for learning, not production  
✅ **Consistent:** Naming conventions throughout  

## Build Requirements 🛠️

| Tool | Purpose |
|------|---------|
| GCC | C compiler (needs i386 target for x86) |
| GNU Make | Build automation |
| Binutils | Linker & assembler |
| QEMU | x86 emulator for testing |

**Windows Users:** Use WSL2 (simplest) or MinGW

**Mac/Linux:** Works directly with standard dev tools

## Files Overview 📄

```
kernel/
├── core/          Scheduler, kernel init (450 lines)
├── mm/            Memory manager (400 lines)
├── fs/            File system, VFS (300 lines)
├── drivers/       Driver framework, PIC, PIT (350 lines)
├── arch/x86/      Boot, GDT, IDT, paging (500 lines)
└── include/       Public headers

docs/              HTML documentation
README.md          Learning guide
ARCHITECTURE.md    Subsystem design (600+ lines)
BUILDING.md        Build instructions
Makefile           Build configuration
```

## Usage Examples 💡

### Build
```bash
cd SimpleOS
make              # Compile kernel → simpleos.elf
```

### Run
```bash
make qemu         # Launch in QEMU with serial output
```

### Clean
```bash
make clean        # Remove build artifacts
```

### Extend
```bash
# Add a new driver (see kernel/drivers/pit.c as template)
# Add a new syscall (modify kernel/core/sched.c)
# Add a new filesystem (implement ops in kernel/fs/)
```

## Learning Outcomes 📖

After studying SimpleOS, you'll understand:

- ✅ How operating systems boot (real mode → protected mode)
- ✅ Process scheduling algorithms
- ✅ Memory management (paging, buddy allocator)
- ✅ Interrupt handling
- ✅ File system abstractions
- ✅ Device driver interfaces
- ✅ x86 CPU modes and segmentation
- ✅ GDT, IDT, and exception handling

## Not Included (By Design)

- Disk I/O (uses RAM instead)
- Networking
- Graphics rendering
- User-mode restrictions (everything in privileged mode for simplicity)
- Multiple cores (single-core focus)

These can be added as extensions once you understand the foundation.

## Next Steps 🎯

1. **Read:** [QUICKSTART.md](QUICKSTART.md) - Get running in 2 minutes
2. **Learn:** [README.md](README.md) - Learning guide
3. **Understand:** [ARCHITECTURE.md](ARCHITECTURE.md) - Deep dive
4. **Explore:** Read source code in [kernel/](kernel/)
5. **Extend:** Add your own driver or syscall

## Related Resources 📚

- **QEMU x86 Boot:** https://wiki.osdev.org/Bare_Bones
- **Intel x86 Manuals:** https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
- **OSDev Wiki:** https://wiki.osdev.org/
- **GCC Inline Assembly:** https://gcc.gnu.org/onlinedocs/gcc/Using-Inline-Assembly-with-C-Code.html

---

**Ready to boot?** → [QUICKSTART.md](QUICKSTART.md)

**Want details?** → [ARCHITECTURE.md](ARCHITECTURE.md)

**Build issues?** → [BUILDING.md](BUILDING.md)
