#include "common/types.h"
#include "common/mem.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "drivers/pic.h"
#include "drivers/term.h"
#include "mem/gdt.h"
#include "task/task.h"
#include "descriptors.h"
#include "idt.h"
#include "irq.h"
#include "x86.h"

#define INTERRUPT_COUNT   256

// Processor exceptions are located in this range of entries in the IVT
#define FIRST_EXCEPTION   0
#define LAST_EXCEPTION    20
#define EXCEPTION_COUNT   (LAST_EXCEPTION-FIRST_EXCEPTION+1)

// Reprograms the PIC to relocate hardware interrupts starting at IVT entry 32:
// IRQ0  -> Interrupt 32
// IRQ1  -> Interrupt 33
// ...
// IRQ15 -> Interrupt 47
#define IRQ_REMAP_OFFSET  32

// Structure of an IDT descriptor. There are 3 types of descriptors:
// task-gates, interrupt-gates, trap-gates.
// See 5.11 of Intel 64 & IA32 architectures software developer's manual for more details.
// For task gates, offset must be 0.
typedef struct {
    uint16_t offset15_0;   // only used by trap and interrupt gates
    uint16_t selector;     // segment selector for trap and interrupt gates; TSS segment
                           // selector for task gates
    uint16_t reserved : 8;
    uint16_t type : 5;
    uint16_t dpl : 2;
    uint16_t p : 1;
    uint16_t offset31_16;  // only used by trap and interrupt gates
} __attribute__((packed)) idt_entry_t;

// CPU context used when saving/restoring context from an interrupt
typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t ebp, edi, esi;
    uint32_t edx, ecx, ebx, eax;
    uint32_t number, error_code;
    uint32_t eip, cs, eflags, esp, ss;
} regs_t;

// Structure describing a pointer to the IDT gate table.
// This format is required by the lidt instruction.
typedef struct {
    uint16_t limit;   // Limit of the table (ie. its size)
    uint32_t base;    // Address of the first entry
} __attribute__((packed)) idt_ptr_t;

// Gates table
static idt_entry_t idt[INTERRUPT_COUNT];
static idt_ptr_t   idt_ptr;

// Loads the IDT specified in argument.
// Defined in idt_asm.s
extern void idt_load(idt_ptr_t *idt_ptr);

// Builds and returns an IDT entry.
// selector is the code segment selector to access the ISR
// offset is the address of the ISR (for task gates, offset must be 0)
// type indicates the IDT entry type
// dpl is the privilege level required to call the associated ISR
static idt_entry_t idt_build_entry(uint16_t selector, uint32_t offset, uint8_t type, uint8_t dpl) {
    idt_entry_t entry;
    entry.offset15_0 = offset & 0xffff;
    entry.selector = selector;
    entry.reserved = 0;
    entry.type = type;
    entry.dpl = dpl;
    entry.p = 1;
    entry.offset31_16 = (offset >> 16) & 0xffff;
    return entry;
}

// Low-level exception handlers.
// These are defined in idt_asm.s
extern void _exception0();
extern void _exception1();
extern void _exception2();
extern void _exception3();
extern void _exception4();
extern void _exception5();
extern void _exception6();
extern void _exception7();
extern void _exception8();
extern void _exception9();
extern void _exception10();
extern void _exception11();
extern void _exception12();
extern void _exception13();
extern void _exception14();
extern void _exception15();
extern void _exception16();
extern void _exception17();
extern void _exception18();
extern void _exception19();
extern void _exception20();

// Low-level hardware interrupt handlers.
// These are defined in idt_asm.s
extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

extern void _syscall_handler();

static char *exception_names[EXCEPTION_COUNT] = {
    "Divide Error",
    "Reserved",
    "NMI Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Intel Reserved1",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Exception",
    "Virtualization Exception"
};

// High-level handler for all exceptions.
void exception_handler(regs_t *regs) {
	uint16_t t = task_get_current_sel();
	task_t* task = get_task_by_selector(t);
	if (task->id >= 0 && task->id < MAX_TASK_COUNT){
		uint8_t *addr_pt = regs->eip;
		*addr_pt = 0xCF;
		term_printf("Task %d terminated\n", task->id);
	} else {
		term_setfgcolor(YELLOW);
		term_setbgcolor(RED);
		term_printf("Exception %d triggered by kernel code: %s (error code 0x%x)!\n", regs->number, exception_names[regs->number], regs->error_code);
		term_printf("Kernel PANIC.\n");
		halt();
	}
}

// High-level handler for all hardware interrupts.
void irq_handler(regs_t *regs) {
    uint_t irq = regs->number;
    pic_eoi(irq);

    handler_t *handler = irq_get_handler(irq);
    if (handler) {
        if (handler->func)
            handler->func();
    }
}

void idt_init() {
    irq_init();

    // Setup the IDT pointer structure.
    idt_ptr.limit = sizeof(idt)-1;
    idt_ptr.base  = (uint32_t)&idt;

    // Clears up the IDT table.
    memsetb(idt, 0, sizeof(idt));

    // IDT entries 0-20: CPU exceptions.
    // Create interrupt handlers for exceptions 0-20.
    // These handlers are located at indices 0-20 in the IVT.
    void (*exceptions[])(void) = {
        _exception0,_exception1,_exception2,_exception3,_exception4,
        _exception5,_exception6,_exception7,_exception8,_exception9,
        _exception10,_exception11,_exception12,_exception13,_exception14,
        _exception15,_exception16,_exception17,_exception18,_exception19,_exception20
    };
    for (int i = FIRST_EXCEPTION; i <= LAST_EXCEPTION; i++) {
        idt[i] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)exceptions[i], TYPE_INTERRUPT_GATE, DPL_KERNEL);
    }

    // IDT entries 32-47: hardware interrupts.
    // Creates interrupt handlers for IRQ0-15.
    // These handlers are located at indices 32-47 in the IVT.
    void (*irqs[])(void) = {
        // Timer (PIT) fires IRQ0
        // Keyboard fires IRQ1
        _irq0,_irq1,_irq2,_irq3,_irq4,_irq5,_irq6,_irq7,_irq8,_irq9,_irq10,_irq11,_irq12,_irq13,_irq14,_irq15
    };
    for (int i = IRQ_FIRST; i <= IRQ_LAST; i++) {
        idt[IRQ_REMAP_OFFSET+i] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t)irqs[i], TYPE_INTERRUPT_GATE, DPL_KERNEL);
    }

    // TODO
    // Add IDT entry 48: system call

	idt[48] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) & _syscall_handler, TYPE_TRAP_GATE, DPL_USER);

    // Loads the IDT.
    idt_load(&idt_ptr);

    term_puts("IDT initialized.\n");
}
