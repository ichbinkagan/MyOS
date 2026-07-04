global irq0
global irq1
global irq_default
global isr_default

extern keyboard_irq

isr_default:
    cli
    hlt
    jmp isr_default

irq0:
    pusha
    mov al, 0x20
    out 0x20, al
    popa
    iret

irq1:
    pusha
    call keyboard_irq
    popa
    iret

irq_default:
    pusha
    mov al, 0x20
    out 0x20, al
    out 0xA0, al
    popa
    iret
