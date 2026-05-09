# SimpleOS - Documentation Index & Navigation Guide

Welcome! This page helps you navigate SimpleOS documentation and get started quickly.

---

## 🚀 START HERE (Choose Your Path)

### ⏱️ Super Quick (5 minutes)
Want to see it boot immediately without understanding everything?

→ **[QUICKSTART.md](QUICKSTART.md)**
- 3-step setup (install tools, build, run)
- See what boot output looks like
- Know how to exit QEMU

---

### 🎓 Learning Path (1-2 hours)
Want to understand what you're looking at?

1. **[GET_STARTED.md](GET_STARTED.md)** (15 min)
   - Complete overview
   - What's included
   - How to extend it

2. **[EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md)** (10 min)
   - Quick facts table
   - 5-minute system tour
   - Architecture layers
   - Learning outcomes

3. **[ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md)** (15 min)
   - Visual system architecture
   - Data flow examples
   - Memory layout
   - Initialization sequence

4. **[ARCHITECTURE.md](ARCHITECTURE.md)** (30+ min - detailed)
   - Deep technical design
   - Each subsystem explained
   - Implementation details
   - Code patterns

---

### 🛠️ Setting Up (Windows Users)
Having trouble building?

→ **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)**
- WSL2 installation (recommended)
- MinGW/MSYS2 alternative
- Docker option
- Troubleshooting

---

### 🔨 Build & Run Issues
Build fails or QEMU won't run?

→ **[BUILDING.md](BUILDING.md)**
- Detailed build procedures
- Compiler flags explained
- Troubleshooting checklist
- Custom configurations

---

### 📖 I Want to Study the Code
Ready to dive into implementation?

→ **[README.md](README.md)** (Learning guide)
- Educational narrative
- Conceptual foundation
- Theory before code

**Then:** Read source files in this order:
1. `kernel/arch/x86/boot.S` - Boot process
2. `kernel/core/kmain.c` - Initialization
3. `kernel/core/sched.c` - Scheduler
4. `kernel/mm/mm.c` - Memory manager
5. `kernel/arch/x86/exceptions.S` - Exception handling
6. `kernel/drivers/` - Device drivers
7. `kernel/fs/` - File system

→ **[SUMMARY.md](SUMMARY.md)** (File reference)
- What each file does
- Line-by-line overview

---

## 📚 Documentation Structure

### New Documentation (Just Created)

| File | Size | Purpose | Read Time |
|------|------|---------|-----------|
| [GET_STARTED.md](GET_STARTED.md) | 3 KB | Complete overview + next steps | 15 min |
| [QUICKSTART.md](QUICKSTART.md) | 4 KB | 2-minute setup + boot output | 5 min |
| [EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md) | 5 KB | What's included, why it matters | 10 min |
| [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) | 6 KB | Visual architecture + data flow | 15 min |
| [WINDOWS_SETUP.md](WINDOWS_SETUP.md) | 3 KB | Windows build tool setup | 5 min |
| **This file** | - | Navigation guide | - |

### Existing Documentation

| File | Size | Purpose | Read Time |
|------|------|---------|-----------|
| [README.md](README.md) | 15 KB | Learning guide + concepts | 30 min |
| [ARCHITECTURE.md](ARCHITECTURE.md) | 20 KB | Technical deep-dive | 45 min |
| [BUILDING.md](BUILDING.md) | 10 KB | Build procedures | 20 min |
| [SUMMARY.md](SUMMARY.md) | 8 KB | File-by-file reference | 15 min |

**Total Documentation:** ~6,500+ lines

---

## 🗺️ Navigation by Topic

### I Want to Know...

**How do I build and run this?**
→ [QUICKSTART.md](QUICKSTART.md) (2 min)

**What's actually in this kernel?**
→ [EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md) (5 min)

**How do I extend it?**
→ [GET_STARTED.md](GET_STARTED.md) → "Extending the Kernel" section

**How does process scheduling work?**
→ [ARCHITECTURE.md](ARCHITECTURE.md) → Search "Scheduler"

**How does memory management work?**
→ [ARCHITECTURE.md](ARCHITECTURE.md) → Search "Memory Manager"

**What's the boot sequence?**
→ [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) → "Initialization Sequence"

**How do interrupts work?**
→ [ARCHITECTURE.md](ARCHITECTURE.md) → Search "Interrupt"

**How do I add a new driver?**
→ [GET_STARTED.md](GET_STARTED.md) → "Add a New Driver"

**Why isn't it building?**
→ [BUILDING.md](BUILDING.md) → "Troubleshooting"

**Can I run this on Windows?**
→ [WINDOWS_SETUP.md](WINDOWS_SETUP.md)

**Where's the code for X?**
→ [SUMMARY.md](SUMMARY.md) → File listing

---

## 🧠 Learning Paths

### Path A: Quick Learner (2 hours)
1. [QUICKSTART.md](QUICKSTART.md) - Get it running (5 min)
2. [EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md) - What it is (10 min)
3. [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) - How it works (15 min)
4. Browse `kernel/core/kmain.c` - See initialization (20 min)
5. Read scheduler section in [ARCHITECTURE.md](ARCHITECTURE.md) (20 min)
6. Try modifying boot message and recompile (20 min)

**Result:** Understand overall structure and can modify kernel

### Path B: Deep Learner (1 week)
1. [README.md](README.md) - Conceptual foundation (1 hour)
2. [ARCHITECTURE.md](ARCHITECTURE.md) - Technical design (1.5 hours)
3. Read source in order:
   - boot.S (30 min)
   - kmain.c (30 min)
   - sched.c (1 hour)
   - mm.c (1 hour)
   - exceptions.S (30 min)
   - drivers/ (1 hour)
4. Hands-on: Add new driver or syscall (2 hours)

**Result:** Deep understanding, can extend kernel significantly

### Path C: Focused Expert (2 weeks)
Pick one subsystem and master it:
1. Scheduler → Read sched.c thoroughly, modify algorithm
2. Memory Manager → Study mm.c, tune buddy allocator
3. File System → Extend VFS, add new filesystem type
4. Drivers → Create new device driver framework

---

## 📖 Code Organization Quick Reference

```
SimpleOS/
├── kernel/
│   ├── arch/x86/
│   │   └─ boot.S, setup.c, exceptions.S, irq.c, link.ld
│   ├── core/
│   │   └─ kmain.c (init), sched.c (scheduling)
│   ├── mm/
│   │   └─ mm.c (memory management)
│   ├── fs/
│   │   └─ vfs.c, ramfs.c (file system)
│   ├── drivers/
│   │   └─ driver.c (framework), pic.c, pit.c, console.c
│   └── include/
│       └─ kernel_types.h, sched.h, mm.h, fs.h, driver.h
│
├── Makefile          ← Build command
├── build.ps1         ← Windows build script
├── README.md         ← Learning guide
├── ARCHITECTURE.md   ← Technical reference
├── SUMMARY.md        ← File index
└── [NEW DOCS]        ← Below
    ├── GET_STARTED.md
    ├── QUICKSTART.md
    ├── EXECUTIVE_SUMMARY.md
    ├── ARCHITECTURE_DIAGRAM.md
    ├── WINDOWS_SETUP.md
    ├── PROJECT_SUMMARY.md
    └── INDEX.md (this file)
```

---

## ✨ Key Features at a Glance

### What This Kernel Has
✅ Boot process (real mode → protected mode)  
✅ Process scheduling (6 priority levels)  
✅ Memory management (buddy allocator + paging)  
✅ Virtual file system (VFS + RAMfs)  
✅ Interrupt handling (exceptions + hardware IRQs)  
✅ Device driver framework  
✅ ~3,500 lines of well-organized code  
✅ Extensive documentation  

### Build & Run
```bash
cd SimpleOS
make              # Compile → simpleos.elf
make qemu         # Run in QEMU
Ctrl+A then X     # Exit QEMU
```

### Time Estimates
- Setup (first time): 10-30 minutes
- First build: 1-2 minutes
- First boot: 30 seconds
- Complete understanding: 20-40 hours

---

## 🎯 Quick Decision Tree

```
┌─ I want to RUN it
│  └─ [QUICKSTART.md] (5 min)
│
├─ I want to UNDERSTAND it
│  ├─ Visually? → [ARCHITECTURE_DIAGRAM.md] (15 min)
│  ├─ Conceptually? → [README.md] (30 min)
│  └─ Technically? → [ARCHITECTURE.md] (45 min)
│
├─ I want to EXTEND it
│  └─ [GET_STARTED.md] → "Extending the Kernel" section
│
├─ I have BUILD PROBLEMS
│  ├─ Windows? → [WINDOWS_SETUP.md]
│  └─ General? → [BUILDING.md]
│
├─ I want a QUICK OVERVIEW
│  └─ [EXECUTIVE_SUMMARY.md] (10 min)
│
└─ I want EVERYTHING
   └─ Read all docs in this order:
      1. This file
      2. QUICKSTART.md
      3. EXECUTIVE_SUMMARY.md
      4. ARCHITECTURE_DIAGRAM.md
      5. README.md
      6. ARCHITECTURE.md
      7. BUILDING.md
      8. Read source code in kernel/
```

---

## 📊 Documentation Statistics

| Category | Value |
|----------|-------|
| **Total docs** | 10 files |
| **Total words** | 15,000+ |
| **Total lines** | 6,500+ |
| **Code comments** | 1,000+ lines |
| **Diagrams** | 8 ASCII art |
| **Code examples** | 20+ |
| **Troubleshooting tips** | 30+ |

---

## 🔗 External Resources

**Learning Operating Systems:**
- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development guide
- [Intel x86 Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html) - CPU instruction reference
- [Bare Bones](https://wiki.osdev.org/Bare_Bones) - Simple OS example
- [GCC Inline Assembly](https://gcc.gnu.org/onlinedocs/gcc/Using-Inline-Assembly-with-C-Code.html) - Assembly in C

**Building on SimpleOS:**
- Look at existing drivers (kernel/drivers/) as templates
- Study sched.c for task management patterns
- Reference mm.c for memory allocation
- Check vfs.c/ramfs.c for filesystem abstraction

---

## ✅ Verification Checklist

Before you start, make sure you have:

- [ ] Read [QUICKSTART.md](QUICKSTART.md)
- [ ] Build tools installed (gcc, make, binutils)
- [ ] QEMU installed (optional but recommended)
- [ ] ~50 MB disk space
- [ ] 10-30 minutes for first build and test
- [ ] Text editor for code exploration

---

## 🎓 What You'll Learn

After working through SimpleOS:

✅ How operating systems boot  
✅ Process scheduling algorithms  
✅ Virtual memory and paging  
✅ Interrupt handling  
✅ Device driver architecture  
✅ File system design  
✅ CPU protection modes  
✅ Memory management techniques  
✅ x86 assembly basics  
✅ Systems programming in C  

---

## 🚀 Get Started Now!

### Step 1: Pick Your Path
- Quick? → [QUICKSTART.md](QUICKSTART.md)
- Learning? → [GET_STARTED.md](GET_STARTED.md)
- Deep dive? → [README.md](README.md)

### Step 2: Install Tools
[WINDOWS_SETUP.md](WINDOWS_SETUP.md) (Windows users)  
Or: `apt install gcc make` (Linux) / `brew install gcc make` (Mac)

### Step 3: Build & Run
```bash
cd SimpleOS
make && make qemu
```

### Step 4: Explore Code
Start with `kernel/core/kmain.c` - the 6-phase initialization

**That's it! You have a complete OS to learn from.** 🎉

---

## 📞 Still Stuck?

| Problem | Solution |
|---------|----------|
| Can't build | [BUILDING.md](BUILDING.md) |
| Windows issues | [WINDOWS_SETUP.md](WINDOWS_SETUP.md) |
| Don't know where to start | This page - follow the decision tree |
| Want to understand everything | [README.md](README.md) → [ARCHITECTURE.md](ARCHITECTURE.md) |
| Want to see it run | [QUICKSTART.md](QUICKSTART.md) |
| Want to modify it | [GET_STARTED.md](GET_STARTED.md) |

---

**Welcome to SimpleOS!**  
Your educational operating system kernel awaits.

**Next step:** Pick a path above and start reading!
