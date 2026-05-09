#!/bin/bash
# SimpleOS - Quick Start Checklist
# Use this to verify your SimpleOS setup and get running quickly

echo "╔════════════════════════════════════════════════════════════╗"
echo "║           SimpleOS - Quick Start Checklist                ║"
echo "║          Building the Central Brain - v1.0.0              ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check functions
check_tool() {
    if command -v $1 &> /dev/null; then
        echo -e "${GREEN}✓${NC} $1 found"
        return 0
    else
        echo -e "${RED}✗${NC} $1 not found (sudo apt-get install $2)"
        return 1
    fi
}

check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1 exists"
        return 0
    else
        echo -e "${RED}✗${NC} $1 missing"
        return 1
    fi
}

# Step 1: Check tools
echo "Step 1: Checking required tools..."
echo "────────────────────────────────────────────────────────────"

TOOLS_OK=true

if ! check_tool "gcc" "gcc"; then TOOLS_OK=false; fi
if ! check_tool "make" "make"; then TOOLS_OK=false; fi
if ! check_tool "ld" "binutils"; then TOOLS_OK=false; fi

# Optional but recommended
echo ""
echo "Optional (for emulation and debugging):"
check_tool "qemu-system-i386" "qemu-system-i386" || true
check_tool "gdb" "gdb" || true

if [ "$TOOLS_OK" = false ]; then
    echo ""
    echo -e "${RED}Missing required tools!${NC}"
    echo "Install GCC and Make first:"
    echo "  Ubuntu: sudo apt-get install gcc make binutils"
    echo "  macOS:  brew install gcc make binutils"
    exit 1
fi

echo -e "${GREEN}All required tools found!${NC}"

# Step 2: Check file structure
echo ""
echo "Step 2: Checking file structure..."
echo "────────────────────────────────────────────────────────────"

FILES_OK=true

# Check headers
if ! check_file "kernel/include/kernel_types.h"; then FILES_OK=false; fi
if ! check_file "kernel/include/sched.h"; then FILES_OK=false; fi
if ! check_file "kernel/include/mm.h"; then FILES_OK=false; fi
if ! check_file "kernel/include/fs.h"; then FILES_OK=false; fi
if ! check_file "kernel/include/driver.h"; then FILES_OK=false; fi

# Check core
echo "Checking core files..."
if ! check_file "kernel/core/kmain.c"; then FILES_OK=false; fi
if ! check_file "kernel/core/sched.c"; then FILES_OK=false; fi

# Check subsystems
echo "Checking subsystems..."
if ! check_file "kernel/mm/mm.c"; then FILES_OK=false; fi
if ! check_file "kernel/fs/vfs.c"; then FILES_OK=false; fi
if ! check_file "kernel/drivers/driver.c"; then FILES_OK=false; fi

# Check x86
echo "Checking x86 files..."
if ! check_file "kernel/arch/x86/boot.S"; then FILES_OK=false; fi
if ! check_file "kernel/arch/x86/setup.c"; then FILES_OK=false; fi
if ! check_file "kernel/arch/x86/irq.c"; then FILES_OK=false; fi

# Check build system
echo "Checking build system..."
if ! check_file "Makefile"; then FILES_OK=false; fi

# Check documentation
echo "Checking documentation..."
check_file "README.md" || true
check_file "ARCHITECTURE.md" || true
check_file "BUILDING.md" || true

if [ "$FILES_OK" = false ]; then
    echo ""
    echo -e "${RED}Some files are missing!${NC}"
    echo "Make sure you're in the SimpleOS directory:"
    echo "  cd SimpleOS"
    exit 1
fi

echo -e "${GREEN}All files present!${NC}"

# Step 3: Build kernel
echo ""
echo "Step 3: Building kernel..."
echo "────────────────────────────────────────────────────────────"

if [ -f "Makefile" ]; then
    if make clean > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} Cleaned old build artifacts"
    fi
    
    if make > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} Kernel built successfully!"
        
        if [ -f "simpleos.elf" ]; then
            SIZE=$(du -h simpleos.elf | cut -f1)
            echo -e "${GREEN}✓${NC} Kernel size: $SIZE"
            file simpleos.elf | head -1
        fi
    else
        echo -e "${RED}✗${NC} Build failed!"
        echo "Run 'make' to see errors"
        exit 1
    fi
else
    echo -e "${RED}✗${NC} Makefile not found"
    exit 1
fi

# Step 4: Ready to run
echo ""
echo "Step 4: Ready to run!"
echo "────────────────────────────────────────────────────────────"
echo ""
echo "Next steps:"
echo "  1. Run in emulator: make qemu"
echo "  2. Read documentation: cat README.md"
echo "  3. Study architecture: cat ARCHITECTURE.md"
echo "  4. Debug with GDB: make debug (then in another terminal: gdb simpleos.elf)"
echo ""

# Check if QEMU is available
if command -v qemu-system-i386 &> /dev/null; then
    echo "QEMU is installed. You can run:"
    echo "  ${YELLOW}make qemu${NC}"
    echo ""
    echo "This will boot the kernel in an emulator. You should see:"
    echo "  - Boot messages"
    echo "  - System initialization"
    echo "  - Task creation"
    echo "  - Process scheduling in action"
    echo ""
else
    echo "${YELLOW}Note:${NC} QEMU not found. For emulation, install:"
    echo "  Ubuntu: sudo apt-get install qemu-system-i386"
    echo "  macOS:  brew install qemu"
    echo ""
fi

# Step 5: Understanding the code
echo "Understanding the kernel:"
echo "────────────────────────────────────────────────────────────"
echo ""
echo "File structure:"
echo "  kernel/include/      - API definitions"
echo "    ├─ kernel_types.h  - Fundamental types and macros"
echo "    ├─ sched.h         - Scheduler interface"
echo "    ├─ mm.h            - Memory manager interface"
echo "    ├─ fs.h            - File system interface"
echo "    └─ driver.h        - Driver framework"
echo ""
echo "  kernel/core/         - Core subsystems"
echo "    ├─ kmain.c         - Kernel startup (200 lines)"
echo "    └─ sched.c         - Task scheduler (450 lines)"
echo ""
echo "  kernel/mm/           - Memory management"
echo "    └─ mm.c            - Physical and virtual memory (400 lines)"
echo ""
echo "  kernel/fs/           - File system"
echo "    ├─ vfs.c           - Virtual file system (300 lines)"
echo "    └─ ramfs.c         - RAM filesystem (250 lines)"
echo ""
echo "  kernel/drivers/      - Device drivers"
echo "    ├─ driver.c        - Driver framework (350 lines)"
echo "    ├─ pic.c           - Interrupt controller (200 lines)"
echo "    └─ pit.c           - Timer device (200 lines)"
echo ""
echo "  kernel/arch/x86/     - x86 architecture"
echo "    ├─ boot.S          - Bootloader (350 lines)"
echo "    ├─ setup.c         - CPU setup: GDT, IDT, paging"
echo "    ├─ irq.c           - IRQ/exception dispatch"
echo "    ├─ exceptions.S    - Exception handlers"
echo "    └─ link.ld         - Memory layout"
echo ""

# Learning path
echo "Recommended learning path:"
echo "────────────────────────────────────────────────────────────"
echo ""
echo "1. Read README.md for overview"
echo "   Time: 10 minutes"
echo "   Learn: What is SimpleOS and why it's useful"
echo ""
echo "2. Review ARCHITECTURE.md for design"
echo "   Time: 20 minutes"
echo "   Learn: How the kernel is structured"
echo ""
echo "3. Study kernel/core/kmain.c"
echo "   Time: 15 minutes"
echo "   Learn: Kernel startup sequence"
echo ""
echo "4. Study kernel/core/sched.c"
echo "   Time: 30 minutes"
echo "   Learn: Task scheduling and context switching"
echo ""
echo "5. Study kernel/mm/mm.c"
echo "   Time: 30 minutes"
echo "   Learn: Virtual memory and paging"
echo ""
echo "6. Study kernel/arch/x86/boot.S"
echo "   Time: 30 minutes"
echo "   Learn: Real mode → Protected mode transition"
echo ""
echo "7. Run in debugger and step through"
echo "   Time: 45 minutes"
echo "   Learn: Interactive execution and state inspection"
echo ""
echo "Total estimated study time: 3 hours (to understand core concepts)"
echo ""

# Verification
echo "Verification:"
echo "────────────────────────────────────────────────────────────"
echo ""
echo -e "${GREEN}✓${NC} SimpleOS is ready!"
echo -e "${GREEN}✓${NC} All components built"
echo -e "${GREEN}✓${NC} Documentation available"
echo -e "${GREEN}✓${NC} Ready to study and extend"
echo ""

echo "Commands cheat sheet:"
echo "────────────────────────────────────────────────────────────"
cat << 'EOF'
make                  # Build kernel
make clean            # Remove build artifacts
make qemu             # Run in QEMU emulator
make debug            # Build with debug symbols
make release          # Optimized build
make docs             # Generate HTML documentation
make disasm           # Disassemble kernel
make symbols          # List kernel symbols
make memmap           # Show memory layout

# For debugging:
make debug
qemu-system-i386 -kernel simpleos.elf -gdb tcp::1234 -S
# In another terminal:
gdb simpleos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue

# To read the code:
less kernel/core/kmain.c
less kernel/core/sched.c
less kernel/mm/mm.c
EOF

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║              SimpleOS Ready for Learning!                 ║"
echo "║         The Central Brain is Built and Ready 🚀          ║"
echo "╚════════════════════════════════════════════════════════════╝"
