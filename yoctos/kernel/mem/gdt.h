#ifndef _GDT_H_
#define _GDT_H_

#include "task/tss.h"

#define KERNEL_TSS_INDEX (5)
#define KERNEL_TSS_SEL (KERNEL_TSS_INDEX*8)

// kernel code and data selectors in the GDT
#define GDT_KERNEL_CODE_SELECTOR   (8 | DPL_KERNEL)
#define GDT_KERNEL_DATA_SELECTOR  (16 | DPL_KERNEL)

// user code and data selectors in the GDT
#define GDT_USER_CODE_SELECTOR    (24 | DPL_USER)
#define GDT_USER_DATA_SELECTOR    (32 | DPL_USER)

// Structure of a GDT descriptor. There are 2 types of descriptors: segments and TSS.
// Section 3.4.5 of Intel 64 & IA32 architectures software developer's manual describes
// segment descriptors while section 6.2.2 describes TSS descriptors.
typedef struct gdt_entry_st {
    uint16_t lim15_0;
    uint16_t base15_0;
    uint8_t base23_16;

    uint8_t type : 4;
    uint8_t s : 1;
    uint8_t dpl : 2;
    uint8_t present : 1;

    uint8_t lim19_16 : 4;
    uint8_t avl : 1;
    uint8_t l : 1;
    uint8_t db : 1;
    uint8_t granularity : 1;

    uint8_t base31_24;
} __attribute__((packed)) gdt_entry_t;

extern void gdt_init();
extern uint_t gdt_entry_to_selector(gdt_entry_t *entry);
extern gdt_entry_t gdt_make_tss(tss_t *tss, uint8_t dpl);

#endif
