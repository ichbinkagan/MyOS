#include "../include/mouse.h"
#include "../include/vga.h"

/* mouse state */
volatile int mouse_x = 400; /* start at center */
volatile int mouse_y = 300;
volatile int mouse_left = 0;
volatile int mouse_right = 0;
volatile int mouse_updated = 0;

/* PS/2 mouse sends 3 bytes per packet */
static unsigned char packet[3];
static int packet_index = 0;

static void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static unsigned char inb(unsigned short port)
{
    unsigned char r;
    __asm__ volatile("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

/* wait until PS/2 controller is ready to receive a command */
static void mouse_wait_write()
{
    int timeout = 100000;
    while (timeout--)
    {
        if (!(inb(0x64) & 0x02))
            return;
    }
}

/* wait until PS/2 controller has data ready */
static void mouse_wait_read()
{
    int timeout = 100000;
    while (timeout--)
    {
        if (inb(0x64) & 0x01)
            return;
    }
}

/* send a command to the mouse */
static void mouse_write(unsigned char cmd)
{
    mouse_wait_write();
    outb(0x64, 0xD4); /* tell controller: next byte goes to mouse */
    mouse_wait_write();
    outb(0x60, cmd); /* send the command */
}

/* read one byte from the mouse */
static unsigned char mouse_read()
{
    mouse_wait_read();
    return inb(0x60);
}

void mouse_init()
{
    /* enable auxiliary device (mouse) */
    mouse_wait_write();
    outb(0x64, 0xA8);

    /* enable mouse interrupts (IRQ12) in controller */
    mouse_wait_write();
    outb(0x64, 0x20); /* read current config byte */
    mouse_wait_read();
    unsigned char config = inb(0x60);
    config |= 0x02;  /* enable IRQ12 */
    config &= ~0x20; /* enable mouse clock */
    mouse_wait_write();
    outb(0x64, 0x60); /* write config byte */
    mouse_wait_write();
    outb(0x60, config);

    /* reset mouse */
    mouse_write(0xFF);
    mouse_read(); /* ACK */
    mouse_read(); /* 0xAA self-test passed */
    mouse_read(); /* 0x00 mouse ID */

    /* set default settings */
    mouse_write(0xF6);
    mouse_read(); /* ACK */

    /* enable data reporting */
    mouse_write(0xF4);
    mouse_read(); /* ACK */

    packet_index = 0;
}

/* IRQ12 handler — called on every mouse event */
void mouse_irq()
{
    /* send EOI to both PICs (mouse is on slave PIC) */
    outb(0xA0, 0x20);
    outb(0x20, 0x20);

    unsigned char data = inb(0x60);

    packet[packet_index++] = data;

    /* mouse sends 3 bytes per movement/click packet */
    if (packet_index < 3)
        return;
    packet_index = 0;

    unsigned char flags = packet[0];
    int dx = (int)(signed char)packet[1];
    int dy = (int)(signed char)packet[2];

    /* bit 3 must always be 1 — if not, packet is out of sync */
    if (!(flags & 0x08))
        return;

    /* update position */
    mouse_x += dx;
    mouse_y -= dy; /* Y is inverted in screen coordinates */

    /* clamp to screen bounds */
    if (mouse_x < 0)
        mouse_x = 0;
    if (mouse_y < 0)
        mouse_y = 0;
    if (mouse_x >= vga_width())
        mouse_x = vga_width() - 1;
    if (mouse_y >= vga_height())
        mouse_y = vga_height() - 1;

    /* button state */
    mouse_left = (flags & 0x01) ? 1 : 0;
    mouse_right = (flags & 0x02) ? 1 : 0;

    mouse_updated = 1;
}