#include <stdint.h>

/* ---- IDT Entry yapısı ---- */
struct IDTEntry
{
    uint16_t base_low;  /* handler adresinin alt 16 biti */
    uint16_t sel;       /* kod segment selector (GDT'deki) */
    uint8_t always0;    /* her zaman 0 */
    uint8_t flags;      /* type + attributes (0x8E = present, ring0, 32-bit interrupt gate) */
    uint16_t base_high; /* handler adresinin üst 16 biti */
} __attribute__((packed));

struct IDTPtr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* 256 giriş: 0-31 CPU exceptions, 32-47 IRQ'lar, geri kalanı kullanılmıyor */
static struct IDTEntry idt[256];
static struct IDTPtr idtp;

/* boot/irq.asm içinde tanımlı olan handler'lar */
extern void isr_default();
extern void irq_default();
extern void irq0();
extern void irq1();
extern void irq12();

/*
 * KOD SEGMENT SELECTOR: 0x10
 * GDT'nizde (boot/kernel.asm veya ayrı bir gdt.asm içinde) kod segmentin
 * hangi offsette olduğuna göre bu değer değişir. Önceki triple-fault
 * analizinden GDT şu şekilde:
 *   0x00 - null descriptor
 *   0x08 - (kullanılmıyor veya farklı bir amaçla)
 *   0x10 - kod segment  <-- BUNU KULLAN
 *   0x18 - veri segment
 */
#define KERNEL_CODE_SEGMENT 0x10

static void set_gate(int n, uint32_t handler)
{
    idt[n].base_low = (uint16_t)(handler & 0xFFFF);
    idt[n].sel = KERNEL_CODE_SEGMENT;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; /* present=1, DPL=00, type=32-bit interrupt gate */
    idt[n].base_high = (uint16_t)((handler >> 16) & 0xFFFF);
}

void idt_init()
{
    idtp.limit = (uint16_t)(sizeof(idt) - 1);
    idtp.base = (uint32_t)&idt;

    /* 0-31: CPU exceptions -> şimdilik hepsi default handler'a gitsin */
    for (int i = 0; i < 32; i++)
        set_gate(i, (uint32_t)isr_default);

    /* 32-47: IRQ0-IRQ15 -> önce hepsini default'a bağla */
    for (int i = 32; i < 48; i++)
        set_gate(i, (uint32_t)irq_default);

    /* sonra ihtiyacımız olanları gerçek handler'lara bağla */
    set_gate(32, (uint32_t)irq0);  /* IRQ0  = timer    */
    set_gate(33, (uint32_t)irq1);  /* IRQ1  = keyboard */
    set_gate(44, (uint32_t)irq12); /* IRQ12 = mouse (slave PIC IRQ4 + 32 + 8 = 44) */

    __asm__ volatile("lidt %0" : : "m"(idtp));
    __asm__ volatile("sti");
}