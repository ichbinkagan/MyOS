#include <stdint.h>

/*
 * The 8259 PIC (Programmable Interrupt Controller) manages hardware IRQs.
 * By default, IRQ0-7 map to CPU interrupts 8-15, which overlap with CPU exceptions.
 * We remap them to interrupts 32-47 so they don't conflict.
 */
#define PIC1 0x20   /* master PIC command port */
#define PIC2 0xA0   /* slave PIC command port */

static void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap()
{
    /* start initialization sequence (ICW1) */
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    /* ICW2: set interrupt vector offsets */
    outb(PIC1 + 1, 0x20);   /* master IRQ0 = interrupt 32 */
    outb(PIC2 + 1, 0x28);   /* slave  IRQ8 = interrupt 40 */

    /* ICW3: tell master PIC that slave is on IRQ2 */
    outb(PIC1 + 1, 0x04);
    outb(PIC2 + 1, 0x02);

    /* ICW4: set 8086 mode */
    outb(PIC1 + 1, 0x01);
    outb(PIC2 + 1, 0x01);

    /*
     * IRQ mask — 1 = masked (disabled), 0 = enabled
     * 0xFC = 11111100 — only IRQ0 (timer) and IRQ1 (keyboard) enabled
     */
    outb(PIC1 + 1, 0xFC);
    outb(PIC2 + 1, 0xFF);   /* disable all slave IRQs */
}