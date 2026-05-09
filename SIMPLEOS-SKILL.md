# SimpleOS Development Skill

This skill provides reusable workflows for common SimpleOS development tasks. Use this as a reference when implementing features, fixing bugs, or extending the kernel.

## Prerequisites

- AGENTS.md read and understood (project overview, code conventions, architecture)
- `make` installed and working: `make qemu` successfully boots the OS
- Familiarity with C and assembly (for x86-specific work)

## Workflow 1: Build, Debug, and Verify Changes

**Goal**: Make a code change, rebuild, test in QEMU, and validate.

### Steps

1. **Edit code**
   - Update the relevant `.c` file in `kernel/*/`
   - Follow naming conventions from AGENTS.md (function names, types, macros)
   - Add inline comments for non-obvious logic

2. **Rebuild**
   ```bash
   make clean
   make
   ```
   - Check for compilation errors (compiler will report exact line)
   - If linking fails, check `kernel/arch/x86/link.ld` for section definitions

3. **Test in QEMU**
   ```bash
   make qemu
   ```
   - Observe boot sequence and task output
   - Use `Ctrl+A X` to exit QEMU gracefully
   - Look for `printk()` debug messages (serial output)

4. **Add debug output if needed**
   - Insert `printk()` calls before and after key operations
   - Use format: `printk("[SUBSYSTEM] Message\n", args)`
   - Example: `printk("[SCHED] Task PID=%d added to queue\n", task->pid);`

5. **Verify result**
   - Confirm QEMU completes initialization (6 phases in kmain.c)
   - Check that first user task runs
   - Validate memory stats output if applicable

### Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `undefined reference to symbol` | Missing function implementation | Implement the function or check header file |
| `error: expected declaration specifiers` | Syntax error (missing semicolon, bracket) | Check line before error; use correct type (s32, u32, etc.) |
| QEMU hangs/freezes | Infinite loop or kernel panic | Add `printk()` to narrow down location; check scheduler logic |
| `KERNEL_ENOMEM` at boot | Memory allocation failed | Increase heap size in `kernel/mm/mm.c` if needed |

---

## Workflow 2: Add a New Kernel Module

**Goal**: Create a new kernel subsystem (e.g., timer manager, IPC system, new file system).

### Steps

1. **Create header file** (`kernel/include/mymodule.h`)
   ```c
   #ifndef KERNEL_MYMODULE_H
   #define KERNEL_MYMODULE_H
   
   #include "kernel_types.h"
   
   // Core API
   KERNEL_STATUS mymodule_init(void);
   KERNEL_STATUS mymodule_operation(args, return_ptr);
   void mymodule_cleanup(void);
   
   #endif
   ```

2. **Create implementation** (`kernel/mymodule/mymodule.c`)
   ```c
   #include "kernel/include/mymodule.h"
   #include "kernel/include/kernel_types.h"
   
   // Static globals with g_ prefix
   static struct mymodule_state g_mymodule_state = {0};
   
   KERNEL_STATUS mymodule_init(void) {
       // Initialize subsystem
       // Return KERNEL_SUCCESS or error code
       return KERNEL_SUCCESS;
   }
   
   KERNEL_STATUS mymodule_operation(args, void **return_ptr) {
       // Check inputs
       if (!return_ptr) return KERNEL_EINVAL;
       
       // Perform operation
       // Use kmalloc/kfree for memory
       
       return KERNEL_SUCCESS;
   }
   ```

3. **Update Makefile**
   - Add `kernel/mymodule/mymodule.c` to `SOURCES` variable
   - No need to modify `OBJECTS` or other rules

4. **Add initialization call** in `kernel/core/kmain.c`
   - Find appropriate phase in 6-phase boot sequence
   - Add call: `status = mymodule_init(); if (status != KERNEL_SUCCESS) return;`
   - Update phase comment if creating new phase

5. **Create documentation**
   - Add section to [ARCHITECTURE.md](ARCHITECTURE.md) under appropriate subsystem
   - Document API, key functions, and design patterns
   - Update [SUMMARY.md](SUMMARY.md) with new files

6. **Test**
   - `make qemu` and verify initialization messages
   - Use `printk()` in `mymodule_init()` to confirm execution: `printk("MyModule initialized\n");`

### Example: Simple Timer Module

```c
// kernel/include/timer.h
#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include "kernel_types.h"

KERNEL_STATUS timer_init(void);
KERNEL_STATUS timer_set_callback(u32 ms, void (*callback)(void));

#endif

// kernel/timer/timer.c
#include "kernel/include/timer.h"

static struct {
    u32 tick_count;
    void (*callback)(void);
} g_timer_state = {0};

KERNEL_STATUS timer_init(void) {
    g_timer_state.tick_count = 0;
    printk("[TIMER] Timer subsystem initialized\n");
    return KERNEL_SUCCESS;
}

KERNEL_STATUS timer_set_callback(u32 ms, void (*callback)(void)) {
    if (!callback) return KERNEL_EINVAL;
    g_timer_state.callback = callback;
    return KERNEL_SUCCESS;
}
```

---

## Workflow 3: Extend the Scheduler

**Goal**: Modify scheduler behavior (e.g., add new priority level, change context switch logic).

### Steps

1. **Read scheduler implementation** (`kernel/core/sched.c`)
   - Understand task states: CREATED → READY → RUNNING → BLOCKED → ZOMBIE
   - Review priority queue implementation (6 levels: 0-5)
   - Study `scheduler_context_switch()` to understand CPU context saving

2. **Identify what to modify**
   - Task queue structure? Modify `struct task_control_block` in header
   - Priority logic? Modify `scheduler_add_task()` and `scheduler_pick_next()`
   - Context switch? Modify `scheduler_context_switch()` (careful: affects CPU registers)

3. **Update header** (`kernel/include/sched.h`)
   - Add new fields if extending `struct task_control_block`
   - Declare new functions if adding scheduler APIs
   - Update documentation comment

4. **Update implementation** (`kernel/core/sched.c`)
   - Modify relevant functions
   - Maintain backward compatibility where possible
   - Add `printk()` debug output for new code paths

5. **Update initialization** if needed
   - Check `kernel/core/kmain.c` phase 2 for scheduler setup
   - Update `scheduler_init()` if new fields require initialization

6. **Test thoroughly**
   - Create test tasks with new priority/state combinations
   - Run `make qemu` and verify behavior
   - Add `printk()` to track task transitions: `printk("[SCHED] Task PID=%d state: %d\n", task->pid, task->state);`

7. **Document changes**
   - Update [ARCHITECTURE.md](ARCHITECTURE.md) scheduler section
   - Explain new priority levels or state transitions
   - Add function comments for new/modified APIs

---

## Workflow 4: Implement a Device Driver

**Goal**: Add a new device driver (e.g., keyboard, timer, disk controller).

### Steps

1. **Create driver header** (`kernel/include/mydevice.h`)
   ```c
   #ifndef KERNEL_MYDEVICE_H
   #define KERNEL_MYDEVICE_H
   
   #include "kernel_types.h"
   #include "kernel/include/driver.h"
   
   #define MYDEVICE_IRQ 5
   
   KERNEL_STATUS mydevice_init(void);
   
   #endif
   ```

2. **Implement driver** (`kernel/drivers/mydevice.c`)
   ```c
   #include "kernel/include/mydevice.h"
   #include "kernel/include/driver.h"
   #include "kernel/include/kernel_types.h"
   
   static KERNEL_STATUS mydevice_driver_init(void) {
       printk("[MYDEVICE] Initializing...\n");
       // Device-specific initialization (I/O ports, memory mapping, etc.)
       return KERNEL_SUCCESS;
   }
   
   static void mydevice_irq_handler(u32 irq_number) {
       printk("[MYDEVICE] IRQ %d received\n", irq_number);
       // Handle interrupt
   }
   
   KERNEL_STATUS mydevice_init(void) {
       struct driver_ops ops = {
           .init = mydevice_driver_init,
           .handle_irq = mydevice_irq_handler,
       };
       
       return driver_register("mydevice", MYDEVICE_IRQ, &ops);
   }
   ```

3. **Register in Makefile**
   - Add `kernel/drivers/mydevice.c` to `SOURCES`

4. **Add to kernel initialization** (`kernel/core/kmain.c`, phase 4)
   ```c
   // Phase 4: Drivers
   printk("Detecting drivers...\n");
   mydevice_init();
   other_device_init();
   ```

5. **Test driver**
   - `make qemu` and observe initialization
   - Trigger interrupt (if applicable) and verify handler is called
   - Use `printk()` to log IRQ handler activity

6. **Document**
   - Update [ARCHITECTURE.md](ARCHITECTURE.md) with driver architecture section
   - Document IRQ number, initialization sequence, and any I/O ports

### Driver Registration Pattern

```c
// Register driver with framework
struct driver_ops my_ops = {
    .init = my_driver_init,           // Called during boot
    .handle_irq = my_irq_handler,     // Called when IRQ fires
};
driver_register("my_device", IRQ_NUMBER, &my_ops);
```

---

## Workflow 5: Debug Memory Issues

**Goal**: Diagnose and fix memory allocation problems, corruption, or leaks.

### Steps

1. **Enable memory debugging** (in `kernel/mm/mm.c`)
   - Add `printk()` statements to `kmalloc()` and `kfree()`
   - Log allocation size, returned pointer, and free address
   - Example: `printk("[MM] kmalloc(%d) -> 0x%x\n", size, (u32)ptr);`

2. **Run with debug output**
   ```bash
   make clean && make && make qemu > qemu_output.log 2>&1
   ```
   - Capture full boot sequence with memory allocations

3. **Analyze patterns**
   - Look for allocations that are never freed
   - Check for multiple allocations at same address (use-after-free)
   - Verify sizes match expectations

4. **Add assertions** if heap corruption suspected
   ```c
   // In kmalloc, before returning
   if (allocated_size < requested_size) {
       printk("[MM] ERROR: heap corruption detected\n");
       return KERNEL_ENOMEM;
   }
   ```

5. **Fix issues**
   - Add missing `kfree()` calls where allocations aren't balanced
   - Fix size mismatches (ensure allocations are power of 2)
   - Check for buffer overflows writing past allocation boundary

6. **Verify fix**
   - Re-run `make qemu` and confirm memory debug output looks correct
   - Verify kernel reaches phase 6 (init task)

### Memory Debugging Patterns

```c
// Track allocation
void *ptr;
KERNEL_STATUS status = kmalloc(256, &ptr);
if (status != KERNEL_SUCCESS) {
    printk("[SUBSYS] Memory allocation failed\n");
    return status;
}
printk("[SUBSYS] Allocated 256 bytes at 0x%x\n", (u32)ptr);

// Use memory...

// Free when done
kfree(ptr);
printk("[SUBSYS] Freed memory at 0x%x\n", (u32)ptr);
```

---

## Workflow 6: Extend the Virtual File System (VFS)

**Goal**: Add a new file system backend (e.g., ext2, network FS) or extend inode operations.

### Steps

1. **Understand VFS abstraction** (`kernel/fs/vfs.c`)
   - Study `struct inode` (file metadata)
   - Study `struct file_ops` (file operations interface)
   - Review RAMFs implementation (`kernel/fs/ramfs.c`) as example

2. **Design new file system**
   - Define inode structure (inherits/wraps `struct inode`)
   - List supported operations: open, read, write, close, delete
   - Plan metadata storage and directory structure

3. **Implement file system** (`kernel/fs/myfs.c`)
   ```c
   #include "kernel/include/fs.h"
   
   static KERNEL_STATUS myfs_open(const char *path, struct file **f) {
       // Create file handle
       // Return KERNEL_SUCCESS or error
   }
   
   static KERNEL_STATUS myfs_read(struct file *f, void *buf, size_t size, size_t *read) {
       // Read from file
       // Update *read with bytes actually read
   }
   
   static KERNEL_STATUS myfs_write(struct file *f, const void *buf, size_t size, size_t *written) {
       // Write to file
   }
   
   static KERNEL_STATUS myfs_close(struct file *f) {
       // Close file handle
   }
   
   KERNEL_STATUS myfs_init(void) {
       struct file_ops ops = {
           .open = myfs_open,
           .read = myfs_read,
           .write = myfs_write,
           .close = myfs_close,
       };
       vfs_register_fs("myfs", &ops);
       return KERNEL_SUCCESS;
   }
   ```

4. **Register in VFS** (in `kernel/core/kmain.c`, phase 3)
   ```c
   myfs_init();
   ```

5. **Test file operations**
   - Open file, read/write data, close
   - Create multiple files and verify isolation
   - Test error conditions (file not found, etc.)

6. **Document**
   - Update [ARCHITECTURE.md](ARCHITECTURE.md) VFS section
   - Explain inode layout and on-disk format (if persistent)

---

## Workflow 7: Update Documentation

**Goal**: Keep documentation synchronized with code changes.

### Steps

1. **Update relevant `.md` files**
   - [README.md](README.md): High-level overview and learning guide
   - [ARCHITECTURE.md](ARCHITECTURE.md): Detailed subsystem architecture
   - [BUILDING.md](BUILDING.md): Build procedures and troubleshooting
   - [SUMMARY.md](SUMMARY.md): File-by-file reference

2. **Link, don't duplicate**
   - If a concept is explained in one file, link to it from others
   - Avoid copying paragraphs across multiple `.md` files
   - Use relative links: `[link text](path/to/file.md#section)`

3. **Update HTML documentation** if user-facing content changed
   - Edit [docs/docs.html](docs/docs.html)
   - Add sections for new features
   - Keep navigation menu in sync
   - Validate HTML syntax (`<div>`, `<h1>`, etc.)

4. **Add inline code comments**
   - Explain non-obvious logic with brief comments above code
   - Document function parameters and return values
   - Use format: `// Purpose: ... Parameters: ... Returns: ...`

5. **Update SUMMARY.md** if file structure changed
   - Add new source files to component section
   - List key functions in each file
   - Note important algorithms or patterns

### Documentation Update Checklist

- [ ] `.md` files updated with conceptual changes
- [ ] Links added to avoid duplication
- [ ] HTML docs updated if user-facing
- [ ] Inline comments added for complex logic
- [ ] SUMMARY.md updated with new/renamed files
- [ ] Build commands unchanged (or updated if applicable)

---

## Troubleshooting

| Problem | Diagnosis | Solution |
|---------|-----------|----------|
| "make: not found" | Build tools missing | Install GNU Make and cross-compiler |
| QEMU doesn't start | QEMU not installed or PATH issue | Install QEMU and verify `which qemu-system-i386` works |
| Kernel hangs on boot | Infinite loop or invalid memory access | Add `printk()` to identify hang point; check paging setup |
| Task doesn't run | Scheduler issue or task allocation failure | Verify `scheduler_add_task()` returns KERNEL_SUCCESS; check task priority |
| File operations fail | VFS not initialized or wrong file system | Ensure `vfs_init()` called in phase 3; check file path format |
| Compiler errors with types | Using wrong type name | Use `s8`/`u32` etc.; check `kernel/include/kernel_types.h` |

---

## Quick Commands

```bash
# Build and test
make clean && make && make qemu

# Build only (no test)
make clean && make

# Clean build artifacts
make clean

# Capture QEMU output
make qemu > output.log 2>&1

# Exit QEMU
# In QEMU window: Ctrl+A, then X
```

---

## See Also

- [AGENTS.md](AGENTS.md): Agent instructions, code conventions, architecture overview
- [ARCHITECTURE.md](ARCHITECTURE.md): Detailed subsystem documentation
- [BUILDING.md](BUILDING.md): Build environment setup
- [README.md](README.md): Learning guide and feature overview
