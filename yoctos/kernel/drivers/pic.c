#include "pmio/pmio.h"
#include "term.h"

#define PIC1_CMD        0x20
#define PIC1_DATA       0x21
#define PIC2_CMD        0xA0
#define PIC2_DATA       0xA1

// End Of Interrupt (reactivate the specified PIC)
#define PIC_EOI         0x20

// More details here: http://wiki.osdev.org/8259_PIC
void pic_init() {
    // By default IRQs 0-7 (master PIC) are mapped to interrupts 0x8-0xF (8-15)
    // and IRQs 8-15 (slave PIC) are mapped to interrupts 0x70-0x77 (112-119).
    // In protected mode, this scheme conflicts with CPU exceptions wich are
    // reserved by the CPU and mapped to interrupts 0 to 31.
    // Consequently, we map IRQs 0-7 to interrupts 32-39 and IRQs 8-15 to interrupts 40-47.

    // Restart both PICs
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // Map IRQs 0-7 to interrupts 32-39
    outb(PIC1_DATA, 32);

    // Map IRQs 8-15 to interrupts 40-47
    outb(PIC2_DATA, 40);

    // Setup PICs cascading
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    term_puts("PIC initialized.\n");
}

void pic_eoi(int irq) {
    // An EOI must also be sent to the slave for IRQs > 7
    if (irq > 7)
        outb(PIC2_CMD, PIC_EOI);
    // Send an EOI to the master
    outb(PIC1_CMD, PIC_EOI);
}
