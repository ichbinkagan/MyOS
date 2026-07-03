#ifndef KEYBOARD_H
#define KEYBOARD_H

/* last scan code received from the keyboard */
extern volatile unsigned char last_scancode;

/* flag — set to 1 by IRQ handler when a new key is ready */
extern volatile int key_ready;

#endif