# SimpleOS - Building and Running Guide

## Quick Start

### 1. Prerequisites

#### Linux/macOS
```bash
# Ubuntu/Debian
sudo apt-get install gcc binutils make

# Linux with cross-compilation (for non-x86 host)
sudo apt-get install gcc-4.9-i686 binutils-i686-linux-gnu

# macOS
brew install i386-elf-gcc i386-elf-binutils

# For QEMU emulation
sudo apt-get install qemu-system-i386
brew install qemu
```

#### Windows
- Download MinGW or use WSL with Linux instructions
- Or use Visual Studio's C++ build tools

### 2. Build Kernel

```bash
# Navigate to project root
cd SimpleOS

# Standard build
make

# Build output: simpleos.elf (kernel executable)
# Size: ~50-100 KB (varies with optimization)

# Verify build succeeded
ls -lh simpleos.elf
file simpleos.elf
```

### 3. Run in Emulator

```bash
# Using QEMU (recommended)
make qemu

# Serial output shown in terminal
# Press Ctrl+A then X to exit QEMU
```

### 4. Expected Output

```
╔════════════════════════════════════════════════════════════╗
║         SimpleOS Kernel v1.0.0 - Starting...              ║
║         Building the Central Brain...                     ║
╚════════════════════════════════════════════════════════════╝

[KERNEL] 🔧 Phase 1: Setting up memory management...
[KERNEL] ✓ Memory manager initialized
Memory Statistics:
  Total Physical: 128 MB
  Free: 128 MB
  Allocated: 0 MB

[KERNEL] 🔧 Phase 2: Initializing process scheduler...
[KERNEL] ✓ Scheduler initialized

[KERNEL] 🔧 Phase 3: Initializing virtual file system...
[KERNEL] ✓ Virtual file system initialized

[KERNEL] 🔧 Phase 4: Setting up device driver framework...
[KERNEL] ✓ Driver framework initialized

[KERNEL] 🔧 Phase 5: Setting up system features...
[KERNEL] ✓ Virtual memory enabled

[KERNEL] 🔧 Phase 6: Creating first user task...
[KERNEL] ✓ Init task created (PID 1)

╔════════════════════════════════════════════════════════════╗
║            ✓ SimpleOS Kernel Boot Complete                ║
║            The Central Brain is Ready!                    ║
╚════════════════════════════════════════════════════════════╝

[INIT] Init task started (PID 1)
[INIT] Creating child tasks...
[INIT] Child tasks created, yielding CPU...
[WORKER-1] Task started (PID 2, priority 2)
[WORKER-2] Task started (PID 3, priority 3)
...
```

---

## Build Details

### Compilation Stages

```bash
# Stage 1: Compile C files
gcc -c kernel/core/sched.c -o kernel/core/sched.o

# Stage 2: Assemble assembly files
gcc -c kernel/arch/x86/boot.S -o kernel/arch/x86/boot.o

# Stage 3: Link object files
ld -T kernel/arch/x86/link.ld -o simpleos.elf *.o

# All done automatically by: make
```

### Compiler Flags

```makefile
-Wall -Wextra          # Enable all warnings
-O2                    # Optimization level 2 (default)
-O3                    # Optimization level 3 (release)
-fno-builtin           # Don't use built-in functions
-ffreestanding         # Kernel environment (no standard library)
-nostdinc              # Don't look for standard headers
-std=c99               # C99 standard
```

### Linker Script (`kernel/arch/x86/link.ld`)

Defines memory layout:
```
0x00000000  - Bootloader
0x00100000  - Kernel code (ENTRY)
0x00100000+ - .text (executable code)
0x????????+ - .rodata (read-only data)
0x????????+ - .data (initialized data)
0x????????+ - .bss (uninitialized data)
```

---

## Build Variations

### Debug Build
```bash
make debug

# Includes:
# - Debug symbols (-g)
# - Optimization disabled (-O0)
# - Debug macros enabled
# - Larger binary size
```

### Release Build
```bash
make release

# Includes:
# - Maximum optimization (-O3)
# - No debug symbols
# - Smaller binary size (~60KB)
```

### Analyze Code
```bash
make analyze

# Runs:
# - cppcheck (static analysis)
# - splint (lint checking)
```

### Generate Documentation
```bash
make docs

# Generates HTML documentation using Doxygen
# Output: docs/html/index.html
```

---

## Emulation Options

### Option 1: QEMU (Recommended)

```bash
# Standard kernel boot
qemu-system-i386 -kernel simpleos.elf \
                 -append "console=com1" \
                 -serial stdio \
                 -m 256M

# From Makefile
make qemu

# With more features
qemu-system-i386 -kernel simpleos.elf \
                 -serial stdio \
                 -m 512M \
                 -enable-kvm \              # Use KVM if available
                 -smp 2                     # 2 CPUs (future)
                 --gdb tcp::1234 -S         # Debug mode
```

### Option 2: Bochs

```bash
# Configure bochs
bochs -f bochsrc.txt

# Or interactive
bochs
# At bochs prompt: file simpleos.elf
#                 run
```

### Option 3: VirtualBox

```bash
# Create bootable ISO
make iso

# Create VM with 256MB RAM
# Set boot disk to simpleos.iso
```

---

## Testing

### Running Tests (Future)

```bash
# Unit tests
make test

# Integration tests
make test-integration

# Performance benchmarks
make benchmark
```

### Memory Leak Detection

```bash
# Build with memory tracking
make CFLAGS="-DDEBUG_MEMORY"

# Compile with Valgrind support (if using simulator)
gcc -g kernel/*.c -o test_kernel
valgrind ./test_kernel
```

---

## Troubleshooting

### Problem: `gcc: command not found`
**Solution:** Install build tools
```bash
# Ubuntu
sudo apt-get install build-essential

# CentOS
sudo yum groupinstall "Development Tools"

# macOS
xcode-select --install
```

### Problem: `make: command not found`
**Solution:** Install make
```bash
# Ubuntu
sudo apt-get install make

# CentOS
sudo yum install make

# macOS
brew install make
```

### Problem: QEMU not found
**Solution:** Install QEMU
```bash
# Ubuntu
sudo apt-get install qemu-system-i386

# CentOS
sudo yum install qemu-system-x86

# macOS
brew install qemu
```

### Problem: Build fails with undefined symbols
**Solution:** 
1. Check all source files are compiled
2. Verify linker script is correct
3. Check function declarations in headers

```bash
# See undefined symbols
nm simpleos.elf | grep " U "
```

### Problem: Kernel won't boot
**Solutions:**
1. Check QEMU output for error messages
2. Rebuild with debug symbols: `make debug`
3. Use GDB debugger to step through boot
4. Verify linker script memory layout

---

## Performance Optimization

### Compile-time Optimization

```bash
# Level 0: No optimization
make CFLAGS="-O0"

# Level 2: Balanced (default)
make CFLAGS="-O2"

# Level 3: Aggressive
make CFLAGS="-O3"

# With profile-guided optimization
make CFLAGS="-O3 -march=native -mtune=native"
```

### Linker Optimization

```bash
# Strip symbols (reduces size)
strip simpleos.elf

# Show size breakdown
objdump -h simpleos.elf
```

### Runtime Optimization

```c
// Critical path: Use inline
static inline void fast_function(void) {
    // ...
}

// Avoid memory allocation in loops
void *ptr = kmalloc(SIZE);
for (int i = 0; i < count; i++) {
    use(ptr);  // Reuse allocation
}

// Use bitwise operations
#define PAGE_SIZE       (1 << 12)      // 4KB
#define PAGE_MASK       (PAGE_SIZE - 1)
```

---

## Customization

### Change Memory Size

Edit `kernel/core/kmain.c`:
```c
mm_init(256 * 1024 * 1024);  /* Was 128MB, now 256MB */
```

### Change CPU Frequency

Edit `kernel/drivers/pit.c`:
```c
static u32 pit_frequency = 100;  /* watts per second */
```

### Change Stack Size

Edit `kernel/arch/x86/setup.c`:
```c
mov $0x90000, %esp  /* Stack address (change if needed) */
```

### Disable Features

```makefile
# In Makefile, comment out subsystems
# FS_SRC := kernel/fs/ramfs.c   # Disable filesystem
# DRIVER_SRC :=                 # Disable drivers
```

---

## Kernel Development Tips

### Adding a New Subsystem

1. Create header file: `kernel/include/new_subsys.h`
2. Create implementation: `kernel/core/new_subsys.c`
3. Add to Makefile: `CORE_SRC += kernel/core/new_subsys.c`
4. Initialize in `kmain.c`: `new_subsys_init();`
5. Compile: `make clean && make`

### Adding a Device Driver

1. Create driver: `kernel/drivers/my_driver.c`
2. Implement struct: `driver_t *my_driver`
3. Register in `kmain.c`: `driver_register(&my_device, my_driver)`
4. Add to Makefile: `DRIVER_SRC += kernel/drivers/my_driver.c`

### Debugging Kernel Panic

1. Build with debug symbols: `make debug`
2. Run in QEMU with GDB: `make qemu` (in one terminal)
3. In another terminal:
   ```bash
   gdb simpleos.elf
   (gdb) target remote :1234
   (gdb) continue
   # System panics, GDB breaks
   (gdb) bt                    # Backtrace
   (gdb) info registers        # CPU state
   (gdb) x /10i $eip          # Disassemble
   ```

---

## Next Steps After Build

1. **Examine Generated Files:**
   ```bash
   file simpleos.elf           # Check binary type
   size simpleos.elf           # Section sizes
   readelf -h simpleos.elf     # ELF header info
   ```

2. **Run Tests:**
   - Boot in QEMU and verify output
   - Check task creation messages
   - Verify no kernel panics

3. **Extend Kernel:**
   - Add system call interface
   - Implement real filesystem driver
   - Add device drivers
   - Create user-space programs

4. **Study Code:**
   - Read scheduler logic
   - Understand memory allocation
   - Study exception handling
   - Learn driver framework

---

## Build System Reference

### Make Targets

| Target | Description |
|--------|-------------|
| `all` | Build kernel (default) |
| `iso` | Create bootable ISO image |
| `qemu` | Run in QEMU emulator |
| `qemu-iso` | Run ISO in QEMU |
| `debug` | Build with debug symbols |
| `release` | Build optimized |
| `analyze` | Run static analysis |
| `docs` | Generate documentation |
| `symbols` | List kernel symbols |
| `disasm` | Disassemble kernel |
| `memmap` | Show memory layout |
| `clean` | Remove build artifacts |
| `distclean` | Remove all generated files |

### Build Variables

```bash
# Custom compiler
make CC=clang

# Custom optimization
make CFLAGS="-O3 -Wall"

# Verbose output
make V=1

# Cross-compile
make CROSS_COMPILE=i386-elf-
```

---

## Additional Resources

- **OSDev Wiki:** http://wiki.osdev.org/ (excellent reference)
- **Intel Manuals:** https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
- **QEMU Docs:** https://qemu.weilnetz.de/doc/manual/
- **Linux Kernel:** https://kernel.org (reference implementation)

---

**Version:** 1.0  
**Last Updated:** 2024
