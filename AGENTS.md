# SimpleOS Agent Instructions

Welcome to SimpleOS, an educational x86 operating system kernel. These instructions help AI agents be immediately productive when working on this codebase.

## Project Overview

**SimpleOS** is a complete, bootable x86 OS kernel (~3,450 LoC) demonstrating core OS concepts:
- **Multitasking**: Priority-based scheduler with round-robin execution (6 priority levels)
- **Memory Management**: Buddy allocator (physical), paging (virtual), 4KB pages
- **File System**: VFS (Virtual File System) with inode-based abstraction
- **Device Drivers**: IRQ-driven driver framework with PIC/PIT support
- **Boot & Architecture**: Full x86 boot sequence, GDT/IDT, exception handling

**Status**: Fully implemented, documented, and tested on QEMU emulation.

## Getting Started

### Build & Run

```bash
make              # Build simpleos.elf (~95 KB)
make qemu         # Run in QEMU emulator
make clean        # Remove build artifacts
make all          # Full rebuild
```

**Requirements:**
- GCC cross-compiler (x86 target)
- GNU Make
- QEMU (for testing)
- No external dependencies (uses only standard C headers)

### Project Structure

| Directory | Purpose | Key Files |
|-----------|---------|-----------|
| `kernel/arch/x86/` | Boot, GDT/IDT, paging, exceptions | `boot.S`, `link.ld`, `setup.c`, `exceptions.S`, `irq.c` |
| `kernel/core/` | Kernel initialization, scheduler, main | `kmain.c`, `sched.c` |
| `kernel/drivers/` | Device driver framework | `driver.c`, `pic.c` (PIC), `pit.c` (PIT) |
| `kernel/fs/` | Virtual File System | `vfs.c`, `ramfs.c` (RAM disk) |
| `kernel/mm/` | Memory management | `mm.c` (buddy allocator, paging) |
| `kernel/include/` | Public headers/API definitions | `kernel_types.h`, `sched.h`, `mm.h`, etc. |
| `docs/` | HTML documentation | `docs.html` (main docs), `style.css`, `script.js` |

## Code Conventions & Patterns

### Type System

SimpleOS uses **shorthand type definitions** (defined in `kernel/include/kernel_types.h`):

```c
s8, s16, s32, s64      // signed integers
u8, u16, u32, u64      // unsigned integers
bool                    // boolean (True/False)
```

**Always use these types**, not raw `int`, `char`, etc.

### Status Codes

Functions return `KERNEL_STATUS` codes (defined in `kernel_types.h`):
- `KERNEL_SUCCESS` (0): Operation succeeded
- `KERNEL_ENOMEM`: Memory allocation failed
- `KERNEL_EINVAL`: Invalid argument
- `KERNEL_EBUSY`: Resource busy
- Negative values = errors

**Pattern**: Check return codes after kernel operations:
```c
KERNEL_STATUS status = kmalloc(size, &ptr);
if (status != KERNEL_SUCCESS) {
    return status;  // propagate error
}
```

### Data Structures

SimpleOS uses **intrusive linked lists** with the `CONTAINER_OF` macro:

```c
// Define list node inside your struct
struct task {
    struct list_node list_node;
    // ... other fields
};

// Access containing struct from list node
struct task *t = CONTAINER_OF(node, struct task, list_node);
```

This pattern avoids pointer chasing and improves cache locality.

### Naming Conventions

- **Functions**: `subsystem_action()` (e.g., `scheduler_add_task()`, `vfs_open_file()`)
- **Structs**: `lowercase_with_underscores` (e.g., `task_control_block`, `buddy_allocator`)
- **Macros**: `UPPERCASE_WITH_UNDERSCORES` (e.g., `CONTAINER_OF`, `PACKED`)
- **Global variables**: `g_` prefix (e.g., `g_current_task`, `g_page_tables`)

### Attributes & Compiler Hints

```c
#define INLINE          __inline__
#define NORETURN        __attribute__((noreturn))
#define PACKED          __attribute__((packed))
#define SECTION(name)   __attribute__((section(name)))
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
```

Use `likely/unlikely` in hot paths (scheduler, memory manager).

### Output & Logging

Use **`printk()`** for all console output:

```c
#include "kernel/include/kernel_types.h"

printk("Task created: PID %d\n", task->pid);
printk("Error: memory allocation failed\n");
```

**Note:** `printk()` output goes to serial console (I/O port 0x3F8).

## Architecture & Key Components

### 1. Initialization Sequence (`kernel/core/kmain.c`)

SimpleOS boots in **6 phases**. Understand this to add new subsystems:

1. **Memory Setup**: Buddy allocator, heap initialization
2. **Scheduler**: Initialize task queues and TCB pool
3. **VFS**: Register file systems (RAMFs)
4. **Drivers**: Detect and register PIC/PIT
5. **Virtual Memory**: Enable paging
6. **Init Task**: Create and run first user task

### 2. Scheduler (`kernel/core/sched.c`)

**Model**: Priority-based with 6 levels (0=lowest, 5=highest), round-robin within priority.

**Key functions**:
- `scheduler_init()`: Initialize scheduler
- `scheduler_add_task()`: Add task to ready queue
- `scheduler_context_switch()`: Switch CPU context
- `scheduler_block_task()`: Block current task

**Task States**: CREATED → READY → RUNNING → BLOCKED → ZOMBIE

### 3. Memory Manager (`kernel/mm/mm.c`)

**Physical Memory**: **Buddy allocator** (pow-of-2 block sizes)
```c
KERNEL_STATUS kmalloc(size_t size, void **ptr);
KERNEL_STATUS kfree(void *ptr);
```

**Virtual Memory**: **Paging** (4 KB pages, 4-level page tables on x86-64)
```c
KERNEL_STATUS vm_map(addr_t virt, addr_t phys, u32 flags);
KERNEL_STATUS vm_unmap(addr_t virt);
```

### 4. VFS (`kernel/fs/vfs.c`)

**Model**: Inode-based file system abstraction.

**Key structures**:
- `struct inode`: File metadata (size, owner, permissions)
- `struct file`: Open file handle
- `struct file_ops`: File operations (read, write, open, close)

**Pattern**: Register file system with operations:
```c
struct file_ops my_fs_ops = {
    .open = my_fs_open,
    .read = my_fs_read,
    .write = my_fs_write,
};
vfs_register_fs("myfs", &my_fs_ops);
```

### 5. Driver Framework (`kernel/drivers/driver.c`)

**Model**: IRQ-driven, with PIC (Programmable Interrupt Controller) and PIT (Programmable Interval Timer).

**Pattern**: Register driver with operations:
```c
struct driver_ops my_driver_ops = {
    .init = my_driver_init,
    .handle_irq = my_driver_irq_handler,
};
driver_register("my_device", IRQ_NUMBER, &my_driver_ops);
```

### 6. x86 Architecture (`kernel/arch/x86/`)

**Boot Process**:
1. `boot.S`: Real mode → Protected mode
2. `setup.c`: GDT, IDT, paging setup
3. `exceptions.S`: Exception handlers
4. `irq.c`: IRQ routing

**Key headers**:
- **GDT**: Segment descriptors (code, data, TSS)
- **IDT**: Interrupt/exception handlers
- **Paging**: 4-level page tables with flags (present, writable, user, etc.)

## Working with Documentation

### Existing Documentation

**Primary docs** (read these first):
- [README.md](README.md) (500+ lines): Learning guide and feature overview
- [ARCHITECTURE.md](ARCHITECTURE.md) (600+ lines): Detailed subsystem architecture
- [BUILDING.md](BUILDING.md) (500+ lines): Build procedures and troubleshooting
- [SUMMARY.md](SUMMARY.md): Complete file-by-file reference

**HTML Documentation**:
- [docs/docs.html](docs/docs.html): User-facing documentation and support info
- Generated from user-friendly content (not API docs)

**Inline Documentation**:
- Section headers: Function group descriptions
- Function comments: Purpose, parameters, return values
- Key algorithms: Detailed explanations (e.g., buddy allocator, context switching)

### Adding or Updating Documentation

**For Code Changes**:
1. Update relevant `.md` file if concepts changed
2. Add inline comments for non-obvious logic
3. Update `SUMMARY.md` if file structure changed

**For User Documentation**:
1. Update [docs/docs.html](docs/docs.html) for user-facing changes
2. Keep consistency with [ARCHITECTURE.md](ARCHITECTURE.md) for technical details
3. Use clear, accessible language (educational focus)

**For HTML Updates**:
- File structure: Navigation, sections with IDs (#docs, #support, #contact, #about, #license)
- Styling: Use class names (already defined in [style.css](docs/style.css))
- Links: Use relative paths for internal navigation
- Contact: Keep [Support section](docs/docs.html#support) updated

## Common Tasks for Agents

### Adding a New Component

1. **Create header** in `kernel/include/mycomponent.h`
2. **Implement** in `kernel/mycomponent/mycomponent.c`
3. **Add initialization** call to phase in `kernel/core/kmain.c`
4. **Register** in appropriate subsystem (e.g., `driver_register()` for drivers)
5. **Update** [ARCHITECTURE.md](ARCHITECTURE.md) and [SUMMARY.md](SUMMARY.md)

### Fixing Bugs

1. **Reproduce**: Run `make qemu` and observe behavior
2. **Trace**: Use `printk()` to add debug output
3. **Fix**: Update code
4. **Verify**: Run `make qemu` again
5. **Update docs**: Note the bug and fix in relevant `.md` files

### Extending Scheduler, Memory Manager, or VFS

1. **Understand current implementation** by reading relevant `.c` file
2. **Check usage patterns** in `kernel/core/kmain.c` and other modules
3. **Maintain compatibility**: Don't break existing APIs
4. **Test thoroughly**: Changes affect entire system
5. **Update** [ARCHITECTURE.md](ARCHITECTURE.md) subsystem section

### Updating Documentation

1. **For .md files**: Edit directly, link to other docs rather than duplicate
2. **For HTML docs**: Update [docs/docs.html](docs/docs.html), ensure valid HTML
3. **For code comments**: Keep focused on "why", not "what"
4. **For README.md**: Target educational audience, add learning value

## Error Handling & Debugging

### Common Error Patterns

**Memory errors**:
```c
if (status == KERNEL_ENOMEM) {
    // Out of memory - handle gracefully
    cleanup_and_return();
}
```

**NULL pointer checks**:
```c
struct task *t = kmalloc(...);
if (!t) return KERNEL_ENOMEM;
```

**Status propagation**:
```c
KERNEL_STATUS status = subsystem_operation();
if (status != KERNEL_SUCCESS) return status;  // fail fast
```

### Debugging with printk()

```c
// Add debug output in hot paths
printk("[SCHED] Adding task PID=%d to priority queue %d\n", task->pid, task->priority);
printk("[MEMORY] Allocated %d bytes at 0x%x\n", size, (u32)ptr);
```

**Tip**: Run `make qemu` and capture output to diagnose issues.

## API Quick Reference

### Memory
- `kmalloc(size_t, void**)`: Allocate memory
- `kfree(void*)`: Free memory

### Scheduler
- `scheduler_init()`: Initialize
- `scheduler_add_task(task)`: Add to ready queue
- `scheduler_run()`: Start scheduling

### VFS
- `vfs_open(path, &file)`: Open file
- `vfs_read(file, buf, size, &read)`: Read data
- `vfs_write(file, buf, size, &written)`: Write data
- `vfs_close(file)`: Close file

### Drivers
- `driver_register(name, irq, ops)`: Register driver
- `driver_unregister(name)`: Unregister driver

### Output
- `printk(format, ...)`: Print to serial console

## Related Skills & Customizations

For AI agents working on specific areas, related customizations include:
- **Architecture Changes**: Refer to `ARCHITECTURE.md` for design decisions
- **Build Issues**: Check `BUILDING.md` for toolchain setup
- **Code Extensions**: Follow patterns in existing components (scheduler, drivers, VFS)
- **Testing**: Validate with `make qemu` and manual inspection of output
- **Documentation**: Keep `.md` files and HTML docs in sync

---

**Last Updated**: 2026-04-21  
**Project Status**: Complete, educational, ready for extension  
**Questions?** See [docs/docs.html](docs/docs.html) support section or review [ARCHITECTURE.md](ARCHITECTURE.md)
