# 🎯 SimpleOS - What to Read First

Welcome! Here's the simplest way to get started with SimpleOS.

---

## ⏱️ Pick Your Path by Available Time

### "I have 5 minutes" ⚡
Just want to see it boot?

**Read:** [QUICKSTART.md](QUICKSTART.md)

1. Install tools (if needed)
2. Run `make && make qemu`
3. Watch it boot
4. Press `Ctrl+A` then `X` to exit

That's it! You've seen a real OS kernel boot.

---

### "I have 30 minutes" 📖
Want to understand what you're looking at?

**Follow this order:**
1. [QUICKSTART.md](QUICKSTART.md) (5 min) - Get it running
2. [EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md) (10 min) - What's included
3. [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) (10 min) - How it's organized
4. Open `kernel/core/kmain.c` and read the comments (5 min) - Understand initialization

**Result:** You understand what just booted and how it's organized.

---

### "I have 2 hours" 🎓
Want to really learn this?

**Follow this order:**
1. [QUICKSTART.md](QUICKSTART.md) (5 min) - Get it running first
2. [GET_STARTED.md](GET_STARTED.md) (20 min) - Complete overview
3. [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) (15 min) - Visual understanding
4. [README.md](README.md) (30 min) - Learning guide (conceptual foundation)
5. Read source code in this order (40 min):
   - `kernel/arch/x86/boot.S` - Boot process
   - `kernel/core/kmain.c` - Initialization
   - `kernel/core/sched.c` - Scheduler
   - `kernel/mm/mm.c` - Memory manager
6. Try modifying something and rebuilding (10 min)

**Result:** Deep understanding of core concepts. Ready to explore subsystems.

---

### "I have a week" 🚀
Want to master it?

**Follow this order:**
1. [README.md](README.md) (1 hour) - Conceptual foundation
2. [ARCHITECTURE.md](ARCHITECTURE.md) (1.5 hours) - Technical design details
3. Read all source code in depth (8 hours):
   - boot.S (1 hour)
   - setup.c (1 hour)
   - kmain.c (1 hour)
   - sched.c (1.5 hours)
   - mm.c (1.5 hours)
   - exceptions.S (0.5 hour)
   - drivers/ (1 hour)
4. Hands-on: Add a new driver or syscall (3 hours)
5. Study ARCHITECTURE.md subsystem sections (2 hours)

**Result:** Complete understanding. Can extend kernel confidently.

---

## 🚀 The Absolute Minimum

Just these 3 things to get started:

### 1. Build Tools (Windows: 10 min)
```powershell
# If on Windows, install WSL2
wsl --install -d Ubuntu
# Restart, then:
wsl -d Ubuntu
sudo apt update && sudo apt install gcc binutils make qemu-system-i386
```

Linux/Mac: You probably already have everything

### 2. Build
```bash
cd SimpleOS
make
```

### 3. Run
```bash
make qemu
```

**Exit:** `Ctrl+A` then `X`

---

## 📚 Documentation Quick Reference

| Document | Read When | Time |
|----------|-----------|------|
| **[QUICKSTART.md](QUICKSTART.md)** | First thing - get it running | 5 min |
| **[EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md)** | Want a quick overview | 10 min |
| **[ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md)** | Want to visualize it | 15 min |
| **[GET_STARTED.md](GET_STARTED.md)** | Want complete picture | 20 min |
| **[README.md](README.md)** | Want conceptual foundation | 30 min |
| **[ARCHITECTURE.md](ARCHITECTURE.md)** | Want technical details | 45 min |
| **[INDEX.md](INDEX.md)** | Want to navigate all docs | 10 min |
| **[BUILDING.md](BUILDING.md)** | Have build issues | 20 min |
| **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** | On Windows, need help | 5 min |

---

## 🎯 Key Points to Understand

### SimpleOS Does This

✅ Boots on real x86 hardware (or QEMU emulator)  
✅ Runs multiple processes with scheduling  
✅ Manages memory with paging  
✅ Provides file system  
✅ Handles interrupts  
✅ Runs device drivers  

### How to Learn It

1. **See it run** first (QUICKSTART.md)
2. **Understand the architecture** (ARCHITECTURE_DIAGRAM.md)
3. **Learn the concepts** (README.md)
4. **Study the code** (source files)
5. **Modify something** (extension guides)

### Where Everything Is

- **Boot code:** `kernel/arch/x86/boot.S`
- **Initialization:** `kernel/core/kmain.c`
- **Scheduling:** `kernel/core/sched.c`
- **Memory:** `kernel/mm/mm.c`
- **Files:** `kernel/fs/`
- **Drivers:** `kernel/drivers/`
- **API definitions:** `kernel/include/`

---

## 🔥 Quick Wins (To Feel Accomplished)

### In 10 minutes:
```bash
make && make qemu
# See boot messages, then Ctrl+A X to exit
# ✅ You've run a real OS kernel!
```

### In 20 minutes:
```bash
# Edit kernel/core/kmain.c line ~50
# Change "SimpleOS Kernel v1.0.0" to "MyOS v1.0.0"
make
make qemu
# ✅ You've customized the kernel!
```

### In 1 hour:
```bash
# Read kernel/core/sched.c
# Understand how task scheduling works
# ✅ You understand process scheduling!
```

---

## ❓ Common Questions

**Q: Will this actually boot?**  
A: Yes! It boots in QEMU. Press `Ctrl+A` then `X` to exit.

**Q: Can I run this on real hardware?**  
A: Yes, but you'd need to create a bootable USB/disk image. For learning, QEMU is perfect.

**Q: What OS am I running?**  
A: x86 32-bit protected mode. It supports multitasking, memory management, file systems, interrupts, and drivers.

**Q: How long to understand everything?**  
A: ~20 hours of careful study. But you can see it run in 5 minutes.

**Q: Can I add features?**  
A: Absolutely! See GET_STARTED.md "Extending the Kernel" section.

**Q: Is this real code?**  
A: 100% real. No simulations. Actually boots and runs.

---

## 🚦 Getting Unstuck

**"I don't know where to start"**  
→ Read [QUICKSTART.md](QUICKSTART.md) (5 minutes)

**"I don't understand the architecture"**  
→ Read [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) (visual overview)

**"Build is failing"**  
→ Read [BUILDING.md](BUILDING.md) or [WINDOWS_SETUP.md](WINDOWS_SETUP.md)

**"I want to understand code"**  
→ Read [README.md](README.md) (conceptual foundation first)

**"What should I read next?"**  
→ Check [INDEX.md](INDEX.md) (navigation guide)

---

## 🎓 Learning Milestones

### Level 1 (5 min) - "It Works"
- ✅ Built the kernel
- ✅ Ran it in QEMU
- ✅ Saw boot messages

### Level 2 (30 min) - "I Understand the Basics"
- ✅ Know 6-phase initialization
- ✅ Understand architecture layers
- ✅ Can identify subsystems

### Level 3 (2 hours) - "I Get It"
- ✅ Know how scheduler works
- ✅ Understand memory management
- ✅ Can read most of the code

### Level 4 (20 hours) - "I Understand Everything"
- ✅ Know all subsystems deeply
- ✅ Can modify and extend kernel
- ✅ Could build similar OS

---

## 🎯 Action Items - DO THIS NOW

### Right Now (Next 5 minutes):
1. ✅ Read [QUICKSTART.md](QUICKSTART.md)
2. ✅ Install build tools (if needed)
3. ✅ Run `make && make qemu`

### Next 30 minutes:
4. ✅ Read [EXECUTIVE_SUMMARY.md](EXECUTIVE_SUMMARY.md)
5. ✅ Read [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md)
6. ✅ Browse `kernel/core/kmain.c`

### Next 2 hours:
7. ✅ Read [README.md](README.md)
8. ✅ Study boot process (boot.S)
9. ✅ Study scheduler (sched.c)

### Next Week:
10. ✅ Study memory manager (mm.c)
11. ✅ Study exception handling
12. ✅ Add a new feature

---

## 📞 Need Help?

| Issue | Solution |
|-------|----------|
| Don't know where to start | THIS PAGE + [QUICKSTART.md](QUICKSTART.md) |
| Build tools missing | [WINDOWS_SETUP.md](WINDOWS_SETUP.md) or [BUILDING.md](BUILDING.md) |
| Build fails | [BUILDING.md](BUILDING.md) troubleshooting |
| Can't understand code | Start with [README.md](README.md) for concepts |
| Want to extend kernel | [GET_STARTED.md](GET_STARTED.md) extensions section |
| Want visual overview | [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) |

---

## 🚀 Let's Go!

**Step 1 Right Now:**  
Open [QUICKSTART.md](QUICKSTART.md) and follow the 3 steps.

**In 5 minutes:**  
You'll see a real OS kernel boot in QEMU.

**That's your starting point.** 🎉

---

_SimpleOS: From scratch to booting in minutes, understanding in hours._
