global irq0
global irq1
global irq_default
global isr_default

extern keyboard_irq

; CPU exception handler (interrupts 0-31)
; if an exception fires with no handler, we triple fault — so we catch them here
isr_default:
    cli                  ; disable interrupts
    hlt                  ; halt — kernel panic
    jmp isr_default      ; loop forever

; IRQ0 — timer interrupt
; we don't use the timer yet, just send EOI and return
irq0:
    pusha                ; save all registers
    mov al, 0x20
    out 0x20, al         ; send End Of Interrupt to PIC
    popa                 ; restore all registers
    iret                 ; return from interrupt

; IRQ1 — keyboard interrupt
; called every time a key is pressed or released
irq1:
    pusha                ; save all registers
    call keyboard_irq    ; call C keyboard handler
    popa                 ; restore all registers
    iret                 ; return from interrupt

; default handler for all other IRQs (2-15)
; just send EOI to both PICs and return
irq_default:
    pusha
    mov al, 0x20
    out 0x20, al         ; send EOI to master PIC
    out 0xA0, al         ; send EOI to slave PIC
    popa
    iret