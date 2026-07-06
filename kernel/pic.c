#include <stdint.h>

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256

#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

/*
 * The 8259 PIC (Programmable Interrupt Controller) manages hardware IRQs.
 * By default, IRQ0-7 map to CPU interrupts 8-15, which overlap with CPU exceptions.
 * We remap them to interrupts 32-47 so they don't conflict.
 */
#define PIC1 0x20 /* master PIC command port */
#define PIC2 0xA0 /* slave PIC command port */

static void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap()
{
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* IRQ0 (timer) + IRQ1 (keyboard) açık */
    outb(PIC1_DATA, 0xFC);

    /* IRQ12 (mouse) açık — bit 4 = 0 */
    /* 0xEF = 11101111 — sadece IRQ12 açık */
    outb(PIC2_DATA, 0xEF);
}