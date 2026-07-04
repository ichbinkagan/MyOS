#include <stdint.h>

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256

/*
 * IDT (Interrupt Descriptor Table) tells the CPU where to jump
 * when an interrupt or exception occurs.
 * Each entry is 8 bytes and describes one interrupt handler.
 */
struct IDTEntry
{
    uint16_t base_lo; /* lower 16 bits of handler address */
    uint16_t sel;     /* code segment selector (always 0x08 in our GDT) */
    uint8_t always0;  /* must be zero */
    uint8_t flags;    /* type and attributes (0x8E = 32-bit interrupt gate) */
    uint16_t base_hi; /* upper 16 bits of handler address */
} __attribute__((packed));

/* pointer structure passed to the lidt instruction */
struct IDTPointer
{
    uint16_t limit; /* size of IDT in bytes minus 1 */
    uint32_t base;  /* address of the IDT */
} __attribute__((packed));

static struct IDTEntry idt[256];
static struct IDTPointer idtp;

/* handler declarations from irq.asm */
extern void irq0();
extern void irq1();
extern void irq_default();
extern void isr_default();

/* fill in one IDT entry with the address of a handler function */
void set_gate(int n, uint32_t handler)
{
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
}

void idt_init()
{
    idtp.limit = (uint16_t)(sizeof(idt) - 1);
    idtp.base = (uint32_t)&idt;

    /* CPU exceptions (0-31) — must all have handlers or we triple fault */
    for (int i = 0; i < 32; i++)
        set_gate(i, (uint32_t)isr_default);

    /* hardware IRQs (32-47) — default handler just sends EOI and returns */
    for (int i = 32; i < 48; i++)
        set_gate(i, (uint32_t)irq_default);

    /* override with our specific handlers */
    set_gate(32, (uint32_t)irq0); /* timer */
    set_gate(33, (uint32_t)irq1); /* keyboard */

    /* load the IDT — "m" means pass the memory address directly */
    __asm__ volatile("lidt %0" : : "m"(idtp));

    /* enable interrupts */
    __asm__ volatile("sti");
}