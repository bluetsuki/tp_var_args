#ifndef _PIC_H_
#define _PIC_H_

// Initialize both PICs by mapping IRQs 0-15 to interrupts 32-47.
extern void pic_init();

// Send an EOI to the PICs given which IRQ was handled.
extern void pic_eoi(int irq);

#endif
