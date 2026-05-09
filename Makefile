# SimpleOS Kernel Build Configuration
#
# This Makefile builds the entire kernel from source
# Supports x86 architecture (can be extended for other archs)

# ============================================================================
# BUILD SETTINGS
# ============================================================================

ARCH        := x86
CROSS_COMPILE := 
CC          := $(CROSS_COMPILE)gcc
CFLAGS      := -Wall -Wextra -O2 -fno-builtin -ffreestanding -nostdinc
CFLAGS      += -Ikernel/include -Iinclude
CFLAGS      += -std=c99
LDFLAGS     := -T kernel/arch/$(ARCH)/link.ld -nostdlib

KERNEL_NAME := simpleos.elf
ISO_NAME    := simpleos.iso

# ============================================================================
# SOURCE FILES
# ============================================================================

# Core kernel sources
CORE_SRC    := kernel/core/kmain.c \
               kernel/core/sched.c

# Architecture-specific sources
ARCH_SRC    := kernel/arch/$(ARCH)/boot.S \
               kernel/arch/$(ARCH)/setup.c \
               kernel/arch/$(ARCH)/irq.c \
               kernel/arch/$(ARCH)/exceptions.c

# Subsystem sources
MM_SRC      := kernel/mm/mm.c

FS_SRC      := kernel/fs/vfs.c \
               kernel/fs/ramfs.c

DRIVER_SRC  := kernel/drivers/driver.c \
               kernel/drivers/pic.c \
               kernel/drivers/pit.c

ALL_SRC     := $(CORE_SRC) $(ARCH_SRC) $(MM_SRC) $(FS_SRC) $(DRIVER_SRC)

OBJECTS     := $(ALL_SRC:.c=.o) $(ALL_SRC:.S=.o)

# ============================================================================
# BUILD TARGETS
# ============================================================================

all: $(KERNEL_NAME)

$(KERNEL_NAME): $(OBJECTS)
	@echo "[LD] Building kernel executable..."
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✓ Kernel built: $@"

%.o: %.c
	@echo "[CC] Compiling $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	@echo "[AS] Assembling $<..."
	$(CC) $(CFLAGS) -x assembler-with-cpp -c -o $@ $<

# ============================================================================
# BOOTABLE IMAGE
# ============================================================================

iso: $(KERNEL_NAME)
	@echo "[BUILD] Creating bootable ISO..."
	mkdir -p iso/boot/grub
	cp $(KERNEL_NAME) iso/boot/
	cp kernel/arch/$(ARCH)/grub.cfg iso/boot/grub/
	grub-mkrescue -o $(ISO_NAME) iso/ 2>/dev/null || \
	    echo "grub-mkrescue not found - install grub-tools"
	@echo "✓ ISO image: $(ISO_NAME)"

# ============================================================================
# EMULATION
# ============================================================================

# Run in QEMU emulator
qemu: $(KERNEL_NAME)
	@echo "[QEMU] Starting emulator..."
	qemu-system-i386 -kernel $(KERNEL_NAME) \
	                  -append "console=com1" \
	                  -serial stdio \
	                  -m 256M \
	                  -enable-kvm

qemu-iso: iso
	@echo "[QEMU-ISO] Starting emulator with ISO..."
	qemu-system-i386 -cdrom $(ISO_NAME) \
	                  -m 256M \
	                  -enable-kvm

# ============================================================================
# BUILD OPTIONS
# ============================================================================

# Debug build with symbols
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean $(KERNEL_NAME)
	@echo "✓ Debug build complete"

# Release build optimized
release: CFLAGS := -Wall -Wextra -O3 -fno-builtin -ffreestanding -nostdinc
release: clean $(KERNEL_NAME)
	@echo "✓ Release build complete"

# ============================================================================
# UTILITIES
# ============================================================================

# Analyze code
analyze:
	@echo "[ANALYSIS] Running static analysis..."
	cppcheck --enable=all kernel/
	splint -boolops kernel/*.c

# Documentation
docs:
	@echo "[DOCS] Generating documentation..."
	doxygen kernel/Doxyfile || echo "Install doxygen for docs"

# Examine kernel symbols
symbols: $(KERNEL_NAME)
	nm $(KERNEL_NAME) | sort

# Disassemble kernel
disasm: $(KERNEL_NAME)
	objdump -d $(KERNEL_NAME) | less

# Memory map
memmap: $(KERNEL_NAME)
	objdump -h $(KERNEL_NAME)

# ============================================================================
# CLEANUP
# ============================================================================

clean:
	@echo "[CLEAN] Removing build artifacts..."
	find . -name "*.o" -delete
	find . -name "*.elf" -delete
	find . -name "*.iso" -delete
	rm -rf iso/
	@echo "✓ Cleaned"

distclean: clean
	@echo "[CLEAN] Removing all generated files..."
	rm -rf build/ docs/html/
	@echo "✓ Distribution clean"

.PHONY: all iso qemu qemu-iso debug release analyze docs symbols disasm memmap clean distclean
