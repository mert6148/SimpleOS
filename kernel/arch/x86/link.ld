/**
 * SimpleOS x86 Linker Script
 * Defines memory layout and section placement for kernel
 */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(boot_start)

SECTIONS
{
    . = 0x100000;  /* Load kernel at 1MB (standard for multiboot) */
    
    /* Bootloader code */
    .boot : {
        *(.boot)
        *(.boot32)
    }
    
    /* Executable code */
    .text : {
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Read-write data */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data (BSS) */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /DISCARD/ : {
        *(.note.GNU-stack)
        *(.gnu_debuglink)
    }
}
