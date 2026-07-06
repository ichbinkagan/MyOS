#include "../include/desktop.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/mouse.h"
#include "../include/window.h"

#define TASKBAR_H 30
#define ICON_SIZE 60
#define ICON_PAD 20

#define CURSOR_W 12
#define CURSOR_H 20

#define ICON1_X ICON_PAD
#define ICON2_X (ICON_PAD * 2 + ICON_SIZE)
#define ICON3_X (ICON_PAD * 3 + ICON_SIZE * 2)
#define ICON_Y 80

static int prev_x = 400;
static int prev_y = 300;

static int dragging_index = -1;
static int drag_offset_x = 0;
static int drag_offset_y = 0;

static void draw_cursor(int x, int y, uint32_t color)
{
    for (int i = 0; i < CURSOR_H; i++)
    {
        int w = CURSOR_W - i / 2;
        if (w < 1)
            w = 1;
        for (int j = 0; j < w; j++)
        {
            vga_pixel(x + j, y + i, color);
        }
    }
}

static void erase_cursor(int x, int y)
{
    for (int i = 0; i < CURSOR_H + 2; i++)
        for (int j = 0; j < CURSOR_W + 2; j++)
        {
            int py = y + i;
            int h = vga_height();
            if (py >= h - TASKBAR_H)
                vga_pixel(x + j, py, COLOR_TASKBAR);
            else if (py < h / 2)
                vga_pixel(x + j, py, COLOR_DARKBLUE);
            else
                vga_pixel(x + j, py, RGB(10, 30, 70));
        }
}

static int point_in_icon(int px, int py, int ix, int iy)
{
    return px >= ix && px <= ix + ICON_SIZE &&
           py >= iy && py <= iy + ICON_SIZE;
}

static void draw_icon(int x, int y, const char *label, uint32_t color)
{
    vga_rect(x, y, ICON_SIZE, ICON_SIZE, color);
    vga_rect_border(x, y, ICON_SIZE, ICON_SIZE, COLOR_WHITE);
    vga_text(x + 4, y + ICON_SIZE + 4, label, COLOR_WHITE, COLOR_DARKBLUE);
}

void draw_taskbar()
{
    int w = vga_width();
    int h = vga_height();
    int ty = h - TASKBAR_H;

    vga_rect(0, ty, w, TASKBAR_H, COLOR_TASKBAR);
    vga_rect(5, ty + 5, 70, 20, COLOR_ACCENT);
    vga_text(10, ty + 9, "MyOS", COLOR_WHITE, COLOR_ACCENT);
    vga_text(w - 80, ty + 9, "00:00", COLOR_WHITE, COLOR_TASKBAR);
}

static void handle_mouse_down(int x, int y)
{
    /* önce açık pencerelere bak */
    int idx = window_hit_test(x, y);

    if (idx >= 0)
    {
        window_bring_to_front(idx);
        idx = MAX_WINDOWS - 1; /* bring_to_front sonrası pencere en sona taşındı */

        if (window_hit_close(idx, x, y))
        {
            window_close(idx);
            dragging_index = -1;
            desktop_init(); /* ekranı temiz baştan çiz */
            return;
        }
        if (window_hit_titlebar(idx, x, y))
        {
            dragging_index = idx;
            drag_offset_x = x - windows[idx].x;
            drag_offset_y = y - windows[idx].y;
        }
        return; /* pencereye tıklandı, ikonlara bakma */
    }

    /* pencereye çarpmadıysa ikonlara bak */
    if (point_in_icon(x, y, ICON1_X, ICON_Y))
        window_open(WIN_FILES, "Files", 220, 130, 260, 140);
    else if (point_in_icon(x, y, ICON2_X, ICON_Y))
        window_open(WIN_TERMINAL, "Terminal", 220, 150, 300, 160);
    else if (point_in_icon(x, y, ICON3_X, ICON_Y))
        window_open(WIN_ABOUT, "About", 220, 170, 220, 110);
}

void desktop_init()
{
    window_init();

    int w = vga_width();
    int h = vga_height();

    vga_rect(0, 0, w, h / 2, COLOR_DARKBLUE);
    vga_rect(0, h / 2, w, h / 2, RGB(10, 30, 70));

    vga_text(w / 2 - 40, 20, "MyOS v0.1", COLOR_WHITE, COLOR_DARKBLUE);

    draw_icon(ICON1_X, ICON_Y, "Files", RGB(50, 100, 200));
    draw_icon(ICON2_X, ICON_Y, "Terminal", RGB(30, 130, 30));
    draw_icon(ICON3_X, ICON_Y, "About", RGB(150, 50, 50));

    draw_taskbar();

    draw_cursor(mouse_x, mouse_y, COLOR_WHITE);
}

void desktop_run()
{
    while (1)
    {
        __asm__ volatile("hlt");

        if (mouse_updated)
        {
            mouse_updated = 0;

            erase_cursor(prev_x, prev_y);

            if (dragging_index >= 0 && mouse_left)
            {
                windows[dragging_index].x = mouse_x - drag_offset_x;
                windows[dragging_index].y = mouse_y - drag_offset_y;
            }

            if (mouse_left && dragging_index < 0)
            {
                handle_mouse_down(mouse_x, mouse_y);
            }
            if (!mouse_left)
            {
                dragging_index = -1;
            }

            draw_taskbar();
            window_draw_all();
            draw_cursor(mouse_x, mouse_y, COLOR_WHITE);

            prev_x = mouse_x;
            prev_y = mouse_y;
        }

        if (key_ready)
        {
            key_ready = 0;
            if (last_scancode == 1)
                desktop_init();
        }
    }
}