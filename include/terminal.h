#ifndef TERMINAL_H
#define TERMINAL_H

/* initialize VGA text mode, clear screen */
void terminal_init();

/* write a single character */
void terminal_putchar(char c);

/* erase the last character (backspace) */
void terminal_backspace();

/* write a null-terminated string */
void terminal_write(const char* str);

#endif