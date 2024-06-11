#ifndef _IRQ_H_
#define _IRQ_H_

#include "common/types.h"

#define IRQ_FIRST    0
#define IRQ_LAST     15

typedef struct {
    void (*func)(void);
    char name[64];
} handler_t;

// Initializes the array of IRQ handlers.
void irq_init();

// Installs a handler for the given IRQ.
// The irq parameter must be in the range [0,15] inclusive.
void irq_install_handler(uint_t irq, handler_t handler);

// Retrieves the handler for a given IRQ.
// The irq parameter must be in the range [0,15] inclusive.
// Returns NULL if there is no handler for the given IRQ.
handler_t *irq_get_handler(uint_t irq);

#endif
