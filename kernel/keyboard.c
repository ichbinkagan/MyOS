#include <stdint.h>
#include "../include/terminal.h"

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256

/* PS/2 keyboard sends scan codes to port 0x60 */
#define KEYBOARD_PORT 0x60

/*
 * These are shared with kernel.c via keyboard.h
 * volatile tells the compiler not to optimize these away
 * because they are modified inside an interrupt handler
 */
volatile unsigned char last_scancode = 0;
volatile int key_ready = 0;

/* read one byte from an I/O port */
static unsigned char inb(unsigned short port)
{
    unsigned char r;
    __asm__ volatile("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

/* write one byte to an I/O port */
static void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

/*
 * IRQ1 handler — called by irq1 in irq.asm on every key event
 * Keep this as short as possible — no C function calls, no complex logic
 * The main loop in kernel.c does the actual processing
 */
void keyboard_irq()
{
    /* read the scan code from the keyboard controller */
    unsigned char sc = inb(KEYBOARD_PORT);

    /* send End Of Interrupt to master PIC — must be done or no more IRQs arrive */
    outb(0x20, 0x20);

    /* high bit set means key release — we only care about key press */
    if (sc & 0x80)
        return;

    /* ignore scan codes we don't have in our keymap */
    if (sc >= 58)
        return;

    /* store the scan code for the main loop to pick up */
    last_scancode = sc;
    key_ready = 1;
}