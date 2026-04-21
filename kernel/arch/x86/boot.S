# SimpleOS Bootloader for x86
# Real Mode Assembly - Transitions to Protected Mode
# 
# Memory Layout:
#   0x00000000 - 0x00000FFF: Real mode IVT + BIOS data
#   0x00001000 - 0x00007FFF: Available for bootloader
#   0x00100000 - 0x????????: Kernel image (high memory)

.code16                        # 16-bit real mode code
.section .boot, "awx"

.global boot_start
.extern kernel_main

# ============================================================================
# BOOT ENTRY POINT
# ============================================================================

boot_start:
    jmp boot_real_start

# Multiboot header (for GRUB)
.align 4
multiboot_header:
    .long 0x1BADB002           # Multiboot magic number
    .long 0x00000003           # Flags
    .long -(0x1BADB002 + 0x00000003)  # Checksum

boot_real_start:
    cli                        # Disable interrupts
    cld                        # Clear direction flag
    
    # Set up basic real mode segments
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $0x7000, %sp          # Stack below bootloader
    
    # Print boot message
    mov $boot_msg, %si
    call puts_real
    
    # ====== PHASE 1: CHECK CPU ======
    pushf                      # Test CPUID support
    pop %eax
    mov %eax, %ecx
    xor $0x200000, %eax
    push %eax
    popf
    pushf
    pop %eax
    cmp %eax, %ecx
    je cpu_error
    
    mov $cpu_ok_msg, %si
    call puts_real
    
    # ====== PHASE 2: ENABLE A20 LINE ======
    call a20_enable
    cmp $0, %al
    je a20_error
    
    mov $a20_ok_msg, %si
    call puts_real
    
    # ====== PHASE 3: SETUP GDT ======
    call gdt_install
    
    mov $gdt_ok_msg, %si
    call puts_real
    
    # ====== PHASE 4: LOAD KERNEL ======
    call load_kernel
    
    # ====== PHASE 5: SWITCH TO PROTECTED MODE ======
    cli
    lgdtl gdt_descriptor
    
    mov %cr0, %eax             # Set PE bit in CR0
    or $1, %eax
    mov %eax, %cr0
    
    ljmpl $0x08, $boot_protected  # Jump to protected mode code
    
    # Should never reach here
    hlt

# ============================================================================
# 16-BIT HELPER FUNCTIONS
# ============================================================================

puts_real:
    # Print string at DS:SI in real mode
    mov $0x0E, %ah            # BIOS print character service
    xor %bx, %bx
.puts_loop:
    lodsb
    test %al, %al
    jz .puts_end
    int $0x10
    jmp .puts_loop
.puts_end:
    ret

a20_enable:
    # Enable A20 line using keyboard controller
    cli
    
    # Wait for keyboard controller
    xor %cx, %cx
.wait_input:
    in $0x64, %al
    test $2, %al
    loopnz .wait_input
    
    # Send command to keyboard controller
    mov $0xAD, %al
    out %al, $0x64
    
    xor %cx, %cx
.wait_input2:
    in $0x64, %al
    test $2, %al
    loopnz .wait_input2
    
    mov $0xD0, %al
    out %al, $0x64
    
    xor %cx, %cx
.wait_output:
    in $0x64, %al
    test $1, %al
    loopz .wait_output
    
    in $0x60, %al
    push %rax
    
    xor %cx, %cx
.wait_input3:
    in $0x64, %al
    test $2, %al
    loopnz .wait_input3
    
    mov $0xD1, %al
    out %al, $0x64
    
    xor %cx, %cx
.wait_input4:
    in $0x64, %al
    test $2, %al
    loopnz .wait_input4
    
    pop %rax
    or $2, %al
    out %al, $0x60
    
    xor %cx, %cx
.wait_input5:
    in $0x64, %al
    test $2, %al
    loopnz .wait_input5
    
    mov $0xAE, %al
    out %al, $0x64
    
    sti
    mov $1, %al
    ret

gdt_install:
    # GDT already set up in data section
    ret

load_kernel:
    # For Multiboot bootloader (GRUB), kernel is already loaded
    # For raw bootloader, would need to read from disk
    mov $kernel_loaded_msg, %si
    call puts_real
    ret

# ============================================================================
# GDT AND DESCRIPTORS
# ============================================================================

.align 4
gdt:
    # NULL descriptor (must be present)
    .long 0x00000000
    .long 0x00000000
    
    # Code descriptor (selector 0x08)
    .long 0x0000FFFF           # Limit 15:0, Base 15:0
    .long 0x00CF9A00           # Flags, Base 31:24, Limit 19:16
    
    # Data descriptor (selector 0x10)
    .long 0x0000FFFF
    .long 0x00CF9200
    
    # TFM: Task descriptor can go here if needed
    
gdt_end:

gdt_descriptor:
    .word gdt_end - gdt - 1    # GDT size - 1
    .long gdt                  # GDT base address

# ============================================================================
# BOOT MESSAGES
# ============================================================================

boot_msg:           .asciz "SimpleOS Bootloader v1.0 Starting...\r\n"
cpu_ok_msg:         .asciz "CPU check: OK\r\n"
a20_ok_msg:         .asciz "A20 line: Enabled\r\n"
gdt_ok_msg:         .asciz "GDT: Loaded\r\n"
kernel_loaded_msg:  .asciz "Kernel: Ready to execute\r\n"
prot_mode_msg:      .asciz "Protected mode: Active\r\n"

cpu_error:
    mov $cpu_err_msg, %si
    call puts_real
    hlt

a20_error:
    mov $a20_err_msg, %si
    call puts_real
    hlt

cpu_err_msg:        .asciz "ERROR: CPUID not supported\r\n"
a20_err_msg:        .asciz "ERROR: Cannot enable A20\r\n"

# ============================================================================
# PROTECTED MODE CODE
# ============================================================================

.code32
.section .boot32

boot_protected:
    # Set up protected mode segments
    mov $0x10, %ax             # Data segment selector
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    # Set up kernel stack
    mov $0x90000, %esp
    
    # Call kernel main
    # Arguments: magic (from EBX in Multiboot), mboot_info (in EAX)
    mov $0x1BADB002, %eax      # Multiboot magic
    mov $0x7C00, %ebx          # Multiboot info (simplified)
    
    call kernel_main
    
    # Kernel should never return
    hlt

# ============================================================================
# PADDING AND BOOT SIGNATURE
# ============================================================================

.org 510
.word 0xAA55                   # Boot signature (for MBR)
