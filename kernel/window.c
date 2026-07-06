#include "../include/window.h"
#include "../include/vga.h"
#include <stddef.h>

Window windows[MAX_WINDOWS];

/* basit strcpy (freestanding ortamda stdlib yok) */
static void str_copy(char *dst, const char *src, int max)
{
    int i = 0;
    while (src[i] && i < max - 1)
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

void window_init()
{
    for (int i = 0; i < MAX_WINDOWS; i++)
        windows[i].open = 0;
}

/* boş bir slot bulup pencereyi orada açar, dizideki indexi döner (-1 = yer yok) */
int window_open(WindowType type, const char *title, int x, int y, int w, int h)
{
    /* aynı tipte zaten açık bir pencere varsa onu öne getir, yenisini açma */
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (windows[i].open && windows[i].type == type)
        {
            window_bring_to_front(i);
            return i;
        }
    }

    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (!windows[i].open)
        {
            windows[i].x = x;
            windows[i].y = y;
            windows[i].w = w;
            windows[i].h = h;
            windows[i].open = 1;
            windows[i].type = type;
            str_copy(windows[i].title, title, sizeof(windows[i].title));
            window_bring_to_front(i);
            return i;
        }
    }
    return -1; /* pencere limiti doldu */
}

void window_close(int index)
{
    if (index < 0 || index >= MAX_WINDOWS)
        return;
    windows[index].open = 0;
}

/* index'teki pencereyi dizinin sonuna taşıyarak "en üstte" olmasını sağlar.
 * Diziyi z-order olarak kullanıyoruz: dizinin sonundaki en üstte çizilir. */
void window_bring_to_front(int index)
{
    if (index < 0 || index >= MAX_WINDOWS - 1)
        return;
    Window tmp = windows[index];
    for (int i = index; i < MAX_WINDOWS - 1; i++)
        windows[i] = windows[i + 1];
    windows[MAX_WINDOWS - 1] = tmp;
}

/* verilen ekran koordinatındaki en üstteki (en son çizilen) pencereyi bul */
int window_hit_test(int px, int py)
{
    for (int i = MAX_WINDOWS - 1; i >= 0; i--)
    {
        if (!windows[i].open)
            continue;
        Window *w = &windows[i];
        if (px >= w->x && px <= w->x + w->w &&
            py >= w->y && py <= w->y + w->h + TITLEBAR_H)
        {
            return i;
        }
    }
    return -1;
}

int window_hit_titlebar(int index, int px, int py)
{
    if (index < 0)
        return 0;
    Window *w = &windows[index];
    return px >= w->x && px <= w->x + w->w - CLOSE_BTN_SZ - 4 &&
           py >= w->y && py <= w->y + TITLEBAR_H;
}

int window_hit_close(int index, int px, int py)
{
    if (index < 0)
        return 0;
    Window *w = &windows[index];
    int bx = w->x + w->w - CLOSE_BTN_SZ - 4;
    int by = w->y + 3;
    return px >= bx && px <= bx + CLOSE_BTN_SZ &&
           py >= by && py <= by + CLOSE_BTN_SZ;
}

static void draw_window_content(Window *w)
{
    int cx = w->x + 8;
    int cy = w->y + TITLEBAR_H + 10;

    switch (w->type)
    {
    case WIN_FILES:
        vga_text(cx, cy, "(no files yet)", COLOR_GRAY, COLOR_DARKGRAY);
        break;
    case WIN_TERMINAL:
        vga_text(cx, cy, "MyOS> _", COLOR_GREEN, COLOR_BLACK);
        break;
    case WIN_ABOUT:
        vga_text(cx, cy, "MyOS v0.1", COLOR_WHITE, RGB(40, 40, 80));
        vga_text(cx, cy + 20, "x86 kernel", COLOR_GRAY, RGB(40, 40, 80));
        vga_text(cx, cy + 40, "by Asturyo", COLOR_GRAY, RGB(40, 40, 80));
        break;
    default:
        break;
    }
}

static uint32_t body_color(WindowType type)
{
    switch (type)
    {
    case WIN_TERMINAL:
        return COLOR_BLACK;
    case WIN_ABOUT:
        return RGB(40, 40, 80);
    default:
        return COLOR_DARKGRAY;
    }
}

void window_draw(int index)
{
    if (index < 0 || index >= MAX_WINDOWS)
        return;
    Window *w = &windows[index];
    if (!w->open)
        return;

    uint32_t bg = body_color(w->type);

    /* title bar */
    vga_rect(w->x, w->y, w->w, TITLEBAR_H, COLOR_ACCENT);
    vga_text(w->x + 6, w->y + 4, w->title, COLOR_WHITE, COLOR_ACCENT);

    /* close button (X) */
    int bx = w->x + w->w - CLOSE_BTN_SZ - 4;
    int by = w->y + 3;
    vga_rect(bx, by, CLOSE_BTN_SZ, CLOSE_BTN_SZ, RGB(180, 40, 40));
    vga_text(bx + 3, by + 1, "x", COLOR_WHITE, RGB(180, 40, 40));

    /* body */
    vga_rect(w->x, w->y + TITLEBAR_H, w->w, w->h, bg);
    vga_rect_border(w->x, w->y, w->w, w->h + TITLEBAR_H, COLOR_WHITE);

    draw_window_content(w);
}

void window_draw_all()
{
    /* dizideki sıra = z-order, en sonda olan en üstte çizilir */
    for (int i = 0; i < MAX_WINDOWS; i++)
        window_draw(i);
}