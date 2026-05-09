/**
 * SimpleOS x86 Exception Stubs
 * Assembly language entry points for CPU exceptions
 * These are called directly by the CPU in protected mode
 */

.code32
.section .text

/* Macro for exceptions without error code */
.macro EXCEPTION_NOERROR num
    .global exc_\num
    exc_\num:
        push $0              /* Dummy error code */
        push $\num           /* Exception number */
        jmp exception_common
.endm

/* Macro for exceptions with error code */
.macro EXCEPTION_ERROR num
    .global exc_\num
    exc_\num:
        push $\num           /* Exception number */
        jmp exception_common
.endm

/* Macro for IRQ handlers */
.macro IRQ_HANDLER num, pic_num
    .global irq_\num
    irq_\num:
        push $\pic_num       /* IRQ number */
        jmp irq_common
.endm

/* ============================================================================
 * EXCEPTION ENTRY STUBS
 * =========================================================================== */

EXCEPTION_NOERROR   0    /* Divide by zero */
EXCEPTION_NOERROR   1    /* Debug */
EXCEPTION_NOERROR   2    /* NMI */
EXCEPTION_NOERROR   3    /* Breakpoint */
EXCEPTION_NOERROR   4    /* Overflow */
EXCEPTION_NOERROR   5    /* Bounds check */
EXCEPTION_NOERROR   6    /* Invalid opcode */
EXCEPTION_NOERROR   7    /* FPU not available */
EXCEPTION_ERROR     8    /* Double fault */
EXCEPTION_NOERROR   9    /* Coprocessor overrun */
EXCEPTION_ERROR     10   /* Invalid TSS */
EXCEPTION_ERROR     11   /* Segment not present */
EXCEPTION_ERROR     12   /* Stack fault */
EXCEPTION_ERROR     13   /* General protection */
EXCEPTION_ERROR     14   /* Page fault */
EXCEPTION_NOERROR   15   /* Reserved */
EXCEPTION_NOERROR   16   /* FPU error */
EXCEPTION_ERROR     17   /* Alignment check */
EXCEPTION_NOERROR   18   /* Machine check */
EXCEPTION_NOERROR   19   /* SIMD error */

/* 20-31: Reserved */
EXCEPTION_NOERROR   20
EXCEPTION_NOERROR   21
EXCEPTION_NOERROR   22
EXCEPTION_NOERROR   23
EXCEPTION_NOERROR   24
EXCEPTION_NOERROR   25
EXCEPTION_NOERROR   26
EXCEPTION_NOERROR   27
EXCEPTION_NOERROR   28
EXCEPTION_NOERROR   29
EXCEPTION_NOERROR   30
EXCEPTION_NOERROR   31

/* ============================================================================
 * IRQ ENTRY STUBS
 * =========================================================================== */

IRQ_HANDLER  0,  0    /* Timer */
IRQ_HANDLER  1,  1    /* Keyboard */
IRQ_HANDLER  2,  2    /* Cascade */
IRQ_HANDLER  3,  3    /* Serial 2 */
IRQ_HANDLER  4,  4    /* Serial 1 */
IRQ_HANDLER  5,  5    /* Parallel 2 */
IRQ_HANDLER  6,  6    /* Floppy */
IRQ_HANDLER  7,  7    /* Parallel 1 */
IRQ_HANDLER  8,  8    /* RTC */
IRQ_HANDLER  9,  9    /* Redirect */
IRQ_HANDLER  10, 10   /* Reserved */
IRQ_HANDLER  11, 11   /* Reserved */
IRQ_HANDLER  12, 12   /* Mouse */
IRQ_HANDLER  13, 13   /* Coprocessor */
IRQ_HANDLER  14, 14   /* ATA primary */
IRQ_HANDLER  15, 15   /* ATA secondary */

/* ============================================================================
 * COMMON EXCEPTION HANDLER
 * =========================================================================== */

exception_common:
    /* Save registers */
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    /* Load kernel data segment */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    /* Stack now contains (top to bottom):
       GS, FS, ES, DS,
       EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,
       exception_num, error_code
    */
    
    /* Call dispatcher with arguments:
       EAX = exception_num (arg 1)
       EBX = error_code (arg 2)
    */
    mov 48(%esp), %eax   /* exception_num */
    mov 52(%esp), %ebx   /* error_code */
    
    call exception_dispatch
    
    /* Restore registers */
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
    /* Pop exception number and error code */
    add $8, %esp
    
    iret

/* ============================================================================
 * COMMON IRQ HANDLER
 * =========================================================================== */

irq_common:
    /* Save registers */
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    /* Load kernel data segment */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    /* Call dispatcher with IRQ number in EAX */
    mov 48(%esp), %eax
    call irq_dispatch
    
    /* Restore registers */
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
    /* Pop IRQ number */
    add $4, %esp
    
    iret

/* ============================================================================
 * SYSTEM CALL ENTRY POINT
 * =========================================================================== */

.global syscall_entry
syscall_entry:
    /* Called via int 0x80 from user space */
    pusha
    
    /* Load kernel data segment */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    
    /* Dispatch to syscall handler
       EAX = syscall number
       EBX = arg1, ECX = arg2, EDX = arg3
    */
    call syscall_handler
    
    popa
    iret
