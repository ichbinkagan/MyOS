#include "../include/terminal.h"
#include "../include/keyboard.h"

/* functions defined in other files */
void pic_remap();
void idt_init();
void shell_run(char* input);

/* input buffer — holds what the user is currently typing */
static char buffer[128];
static int  idx = 0;

/*
 * Scan code to ASCII keymap.
 * Index = PS/2 scan code, value = ASCII character.
 * 0 means we ignore that key.
 */
static char keymap[58] = {
    0,   27,                                      /* 0=none, 1=ESC */
    '1','2','3','4','5','6','7','8','9','0','-','=','\b',  /* 2-14 */
    '\t',                                         /* 15=TAB */
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',  /* 16-28 */
    0,                                            /* 29=CTRL */
    'a','s','d','f','g','h','j','k','l',';','\'','`',      /* 30-41 */
    0, '\\',                                      /* 42=LSHIFT, 43 */
    'z','x','c','v','b','n','m',',','.','/',      /* 44-53 */
    0, '*', 0, ' '                                /* 54=RSHIFT, 55-57 */
};

void kmain()
{
    terminal_init();
    terminal_write("MyOS Booted!\n");
    terminal_write("MyOS> ");

    /* set up hardware interrupt routing before enabling interrupts */
    pic_remap();
    idt_init();   /* also calls sti to enable interrupts */

    while (1)
    {
        /* sleep until the next interrupt arrives */
        __asm__ volatile ("hlt");

        /* keyboard IRQ handler sets key_ready = 1 when a key is pressed */
        if (!key_ready) continue;
        key_ready = 0;

        unsigned char sc = last_scancode;
        if (sc >= 58) continue;   /* out of keymap range */

        char c = keymap[sc];
        if (!c) continue;         /* unmapped key */

        if (c == '\n') {
            /* Enter pressed — null-terminate and send to shell */
            buffer[idx] = 0;
            terminal_write("\n");
            shell_run(buffer);
            idx = 0;              /* reset buffer for next command */
        }
        else if (c == '\b') {
            /* Backspace — remove last character from buffer and screen */
            if (idx > 0) {
                idx--;
                terminal_backspace();
            }
        }
        else {
            /* regular character — add to buffer and echo to screen */
            if (idx < 127) {
                buffer[idx++] = c;
                char s[2] = {c, 0};
                terminal_write(s);
            }
        }
    }
}