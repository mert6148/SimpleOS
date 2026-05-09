# SimpleOS - Windows Setup & Build Guide

SimpleOS is designed for Unix/Linux. On Windows, use **WSL2 (Windows Subsystem for Linux)** for the smoothest experience, or **MinGW** for native Windows building.

---

## Option 1: WSL2 (Recommended) ⭐

WSL2 gives you a full Linux environment integrated with Windows.

### Step 1: Install WSL2
```powershell
# Open PowerShell as Administrator and run:
wsl --install -d Ubuntu
```

This installs Ubuntu on WSL2. Restart your computer when prompted.

### Step 2: Install Build Tools in WSL
```bash
# Open your WSL terminal and run:
sudo apt update
sudo apt install -y gcc binutils make git

# Verify installation
gcc --version
make --version
```

### Step 3: Access Your SimpleOS Directory
```bash
# Your Windows files are at /mnt/c/Users/...
cd /mnt/c/Users/mertd/OneDrive/Masaüstü/SimpleOS

# Or create a symlink for easier access
ln -s /mnt/c/Users/mertd/OneDrive/Masaüstü/SimpleOS ~/simpleos
cd ~/simpleos
```

### Step 4: Build and Run
```bash
# Build the kernel
make

# Run in QEMU (install if needed: sudo apt install qemu-system-i386)
make qemu
```

**To exit QEMU:** Press `Ctrl+A` then `X`

---

## Option 2: MinGW/MSYS2 (Native Windows)

### Install MSYS2
1. Download from https://www.msys2.org/
2. Run the installer
3. In MSYS2 terminal:
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-binutils make
pacman -S mingw-w64-i686-gcc  # for 32-bit x86 cross-compilation
```

### Build SimpleOS
```bash
cd /c/Users/mertd/OneDrive/Masaüstü/SimpleOS
make
```

---

## Option 3: Docker (Cross-Platform)

If you have Docker installed:

```dockerfile
# Create Dockerfile in project root
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y gcc binutils make qemu-system-i386
WORKDIR /work
CMD ["bash"]
```

Build and run:
```bash
docker build -t simpleos .
docker run -it -v C:\Users\mertd\OneDrive\Masaüstü\SimpleOS:/work simpleos
make
make qemu
```

---

## Install QEMU for Windows (Optional - if you want GUI)

Download from: https://www.qemu.org/download/

Or via Chocolatey:
```powershell
choco install qemu
```

Then run:
```bash
make qemu
```

---

## Troubleshooting

### "gcc: not found"
- Make sure you've installed the build tools in WSL/MSYS2
- Check your PATH includes the compiler directory

### "qemu-system-i386: not found"
- Install QEMU in your WSL/MSYS2 environment
- On WSL Ubuntu: `sudo apt install qemu-system-i386`

### Build succeeds but kernel won't boot
- Check boot messages with `make qemu`
- Serial output shows debug messages (watch for errors)

---

## Quick Reference

| Task | WSL Command |
|------|-------------|
| Build kernel | `make` |
| Run in QEMU | `make qemu` |
| Create bootable ISO | `make iso` |
| Clean build artifacts | `make clean` |

---

**Recommended:** Use WSL2 + Ubuntu for the best experience. It's free, integrated, and identical to native Linux.

Still stuck? See [BUILDING.md](BUILDING.md) for detailed architecture-specific info.
