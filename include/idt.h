#ifndef IDT_H
#define IDT_H

void idt_init();
void set_idt_gate(int n, unsigned int handler);

#endif