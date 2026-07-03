#include <stdint.h>

/*
 * VGA text mode buffer starts at physical address 0xB8000
 * Each character takes 2 bytes:
 *   byte 0 = ASCII character
 *   byte 1 = color attribute (high nibble = background, low nibble = foreground)
 */
static uint16_t* const VGA = (uint16_t*)0xB8000;

static int     row   = 0;     /* current cursor row (0-24) */
static int     col   = 0;     /* current cursor column (0-79) */
static uint8_t color = 0x07;  /* light gray on black */

/* clear the screen and reset cursor to top-left */
void terminal_init()
{
    for (int y = 0; y < 25; y++)
        for (int x = 0; x < 80; x++)
            VGA[y * 80 + x] = (color << 8) | ' ';

    row = col = 0;
}

/* write a single character to the screen at the current cursor position */
void terminal_putchar(char c)
{
    if (c == '\n') {
        /* newline — move to the beginning of the next row */
        row++;
        col = 0;
        return;
    }

    /* write color + character as a 16-bit value into VGA memory */
    VGA[row * 80 + col] = (color << 8) | c;
    col++;

    /* wrap to next line if we hit the right edge */
    if (col >= 80) {
        col = 0;
        row++;
    }
}

/* erase the last character (used for backspace) */
void terminal_backspace()
{
    if (col > 0) {
        col--;
    } else if (row > 0) {
        /* go to end of previous row */
        row--;
        col = 79;
    }

    /* overwrite the character with a space */
    VGA[row * 80 + col] = (color << 8) | ' ';
}

/* write a null-terminated string to the screen */
void terminal_write(const char* str)
{
    while (*str)
        terminal_putchar(*str++);
}