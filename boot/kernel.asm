global _start
extern kmain

; Multiboot header constants — GRUB uses these to identify a bootable kernel
MBOOT_MAGIC    equ 0x1BADB002
MBOOT_FLAGS    equ 0x0
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .text
align 4
    ; Multiboot header must be in the first 8KB of the binary
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

_start:
    mov esp, stack_top   ; set up the stack pointer before calling C
    call kmain           ; jump to C kernel

hang:
    cli                  ; disable interrupts
    hlt                  ; halt the CPU
    jmp hang             ; if CPU wakes up, halt again

section .bss
align 16
stack_bottom:
    resb 16384           ; reserve 16KB for the stack
stack_top: