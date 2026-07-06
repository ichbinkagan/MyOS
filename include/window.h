#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#define MAX_WINDOWS 8
#define TITLEBAR_H 20
#define CLOSE_BTN_SZ 14

typedef enum
{
    WIN_NONE = 0,
    WIN_FILES,
    WIN_TERMINAL,
    WIN_ABOUT
} WindowType;

typedef struct
{
    int x, y, w, h;
    char title[32];
    int open; /* 1 = açık, 0 = kapalı/kullanılmıyor */
    WindowType type;
} Window;

extern Window windows[MAX_WINDOWS];

void window_init();
int window_open(WindowType type, const char *title, int x, int y, int w, int h);
void window_close(int index);
void window_draw(int index);
void window_draw_all();
int window_hit_test(int px, int py); /* -1 = hiçbir pencereye çarpmadı */
int window_hit_titlebar(int index, int px, int py);
int window_hit_close(int index, int px, int py);
void window_bring_to_front(int index);

#endif