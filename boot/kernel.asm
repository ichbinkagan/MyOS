global _start
extern kmain

; bit 0 = align
; bit 1 = memory map  
; bit 2 = video mode request
MBOOT_MAGIC    equ 0x1BADB002
MBOOT_FLAGS    equ 0x00000007
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .text
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

    ; video fields (only used when bit 2 is set)
    dd 0          ; header_addr   (0 = use ELF)
    dd 0          ; load_addr
    dd 0          ; load_end_addr
    dd 0          ; bss_end_addr
    dd 0          ; entry_addr
    dd 0          ; mode_type: 0 = linear framebuffer
    dd 800        ; width
    dd 600        ; height
    dd 32         ; depth (bits per pixel)

_start:
    mov esp, stack_top
    push ebx
    call kmain

hang:
    cli
    hlt
    jmp hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top: