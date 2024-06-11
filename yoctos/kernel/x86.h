#ifndef _X86_H_
#define _X86_H_

// Disable hardware interrupts.
static inline void cli() {
    asm volatile("cli");
}

// Enable hardware interrupts.
static inline void sti() {
    asm volatile("sti");
}

// Halt the processor.
// External interrupts wake up the CPU, hence the cli instruction.
static inline void halt() {
    while (1) asm volatile("cli\nhlt");
}

#endif
