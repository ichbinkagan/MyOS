#ifndef VGA_H
#define VGA_H

#include <stdint.h>

/* initialize framebuffer from multiboot info */
int vga_init(uint32_t *mboot);

/* draw a single pixel */
void vga_pixel(int x, int y, uint32_t color);

/* draw a filled rectangle */
void vga_rect(int x, int y, int w, int h, uint32_t color);

/* draw a rectangle border only */
void vga_rect_border(int x, int y, int w, int h, uint32_t color);

/* draw a single character */
void vga_char(int x, int y, char c, uint32_t fg, uint32_t bg);

/* draw a string */
void vga_text(int x, int y, const char *str, uint32_t fg, uint32_t bg);

/* screen dimensions */
int vga_width();
int vga_height();

/* fill entire screen with one color */
void vga_clear(uint32_t color);

/* pack RGB into a 32-bit color value */
#define RGB(r, g, b) ((uint32_t)(((r) << 16) | ((g) << 8) | (b)))

/* common colors */
#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_RED RGB(200, 50, 50)
#define COLOR_GREEN RGB(50, 200, 50)
#define COLOR_BLUE RGB(30, 80, 160)
#define COLOR_DARKBLUE RGB(10, 40, 90)
#define COLOR_GRAY RGB(180, 180, 180)
#define COLOR_DARKGRAY RGB(60, 60, 60)
#define COLOR_TASKBAR RGB(20, 20, 20)
#define COLOR_ACCENT RGB(0, 120, 215)

#endif