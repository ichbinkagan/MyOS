#include "../include/terminal.h"

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256

void shell_run(char *input);

static char buffer[128];
static int idx = 0;

/* correct PS/2 scan code set 1 keymap */
static char keymap[128] = {
    0, 27,                                                            /* 0x00, 0x01=ESC */
    '1', '2', '3', '4', '5', '6',                                     /* 0x02-0x07 */
    '7', '8', '9', '0', '-', '=', '\b',                               /* 0x08-0x0E */
    '\t',                                                             /* 0x0F=TAB */
    'q', 'w', 'e', 'r', 't', 'y',                                     /* 0x10-0x15 */
    'u', 'i', 'o', 'p', '[', ']', '\n',                               /* 0x16-0x1C */
    0,                                                                /* 0x1D=CTRL */
    'a', 's', 'd', 'f', 'g', 'h',                                     /* 0x1E-0x23 */
    'j', 'k', 'l', ';', '\'', '`',                                    /* 0x24-0x29 */
    0, '\\',                                                          /* 0x2A=LSHIFT, 0x2B */
    'z', 'x', 'c', 'v', 'b', 'n', 'm',                                /* 0x2C-0x32 */
    ',', '.', '/',                                                    /* 0x33-0x35 */
    0, '*',                                                           /* 0x36=RSHIFT, 0x37 */
    0, ' ',                                                           /* 0x38=ALT, 0x39=SPACE */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                     /* F1-F10 */
    0, 0,                                                             /* NUMLOCK, SCROLL */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                            /* numpad */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* rest */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0};

static unsigned char inb(unsigned short port)
{
    unsigned char r;
    __asm__ volatile("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

static void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void kmain()
{
    terminal_init();
    terminal_write("MyOS Booted!\n");
    terminal_write("MyOS:/> ");

    /* mask all IRQs — pure polling, no interrupts needed */
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    /* flush keyboard buffer */
    while (inb(0x64) & 0x01)
        inb(0x60);

    unsigned char prev = 0;

    while (1)
    {
        unsigned char status = inb(0x64);
        if (!(status & 0x01))
            continue;

        unsigned char sc = inb(0x60);

        /* key release */
        if (sc & 0x80)
        {
            prev = 0;
            continue;
        }

        /* debounce */
        if (sc == prev)
            continue;
        prev = sc;

        if (sc >= 128)
            continue;

        char c = keymap[sc];
        if (!c)
            continue;

        if (c == '\n')
        {
            buffer[idx] = 0;
            terminal_write("\n");
            shell_run(buffer);
            idx = 0;
        }
        else if (c == '\b')
        {
            if (idx > 0)
            {
                idx--;
                terminal_backspace();
            }
        }
        else
        {
            if (idx < 127)
            {
                buffer[idx++] = c;
                char s[2] = {c, 0};
                terminal_write(s);
            }
        }
    }
}