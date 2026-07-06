#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

void mouse_init();
void mouse_irq();

/* current mouse state */
extern volatile int mouse_x;
extern volatile int mouse_y;
extern volatile int mouse_left;  /* 1 = left button pressed */
extern volatile int mouse_right; /* 1 = right button pressed */
extern volatile int mouse_updated;

#endif