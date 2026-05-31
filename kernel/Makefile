# SimpleOS Educational x86 Kernel Build
# Build this kernel and boot it in QEMU with one command.

CC ?= gcc
QEMU ?= qemu-system-i386

CFLAGS = -std=gnu99 -m32 -ffreestanding -fno-builtin -fno-stack-protector -Iinclude -g -O0 -Wall -Wextra
LDFLAGS = -m32 -nostdlib -T arch/x86/link.ld

SRC_C = arch/x86/setup.c \
	core/kmain.c \
	core/sched.c \
	mm/mm.c \
	fs/vfs.c \
	drivers/driver.c \
	drivers/pic.c \
	drivers/pit.c \
	programs/demo_tasks.c
SRC_ASM = arch/x86/boot.S \
	arch/x86/exceptions.S

OBJDIR = build
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_C)) \
	$(patsubst %.S,$(OBJDIR)/%.o,$(SRC_ASM))
TARGET = $(OBJDIR)/kernel.bin

.PHONY: all run clean

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)/arch/x86 $(OBJDIR)/core $(OBJDIR)/mm $(OBJDIR)/fs $(OBJDIR)/drivers

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.S | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

run: $(TARGET)
	$(QEMU) -kernel $(TARGET) -serial stdio -display none -m 128M

clean:
	rm -rf $(OBJDIR)
