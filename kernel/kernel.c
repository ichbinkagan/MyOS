#include "../include/terminal.h"
#include "../include/vga.h"
#include "../include/desktop.h"
#include "../include/mouse.h"

void pic_remap();
void idt_init();

struct MBootInfo
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint32_t framebuffer_addr_lo;
    uint32_t framebuffer_addr_hi;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
} __attribute__((packed));

void kmain(uint32_t *mboot_info)
{
    pic_remap();
    idt_init();
    mouse_init();

    if (vga_init(mboot_info))
    {
        desktop_init();
        desktop_run();
    }
    else
    {
        terminal_init();
        terminal_write("VGA FAIL - text mode\n");
        terminal_write("MyOS> ");
        while (1)
            __asm__ volatile("hlt");
    }
}