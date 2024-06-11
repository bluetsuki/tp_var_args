#include "common/types.h"
#include "common/mem.h"
#include "task/task.h"
#include "gdt.h"
#include "descriptors.h"

// Descriptor system bit (S)
// For code or data segments
#define S_CODE_OR_DATA   1
// For TSS segment, LDT, call gate, interrupt gate, trap gate, task gate
#define S_SYSTEM   0

// D/B bit
#define DB_SEG  1
#define DB_SYS  0

// Structure describing a pointer to the GDT descriptor table.
// This format is required by the lgdt instruction.
typedef struct gdt_ptr_st {
    uint16_t limit;    // Limit of the table (ie. its size)
    uint32_t base;     // Address of the first entry
} __attribute__((packed)) gdt_ptr_t;

// Macro used to convert the index of a segment descriptor in the GDT into a segment selector.
#define GDT_INDEX_TO_SELECTOR(idx) ((idx) << 3)

// Implemented in gdt_asm.s
extern void gdt_load(gdt_ptr_t *gdt_ptr);

// The first five GDT entries are used for:
//   0: NULL
//   1: kernel code
//   2: kernel data
//   3: user code
//   4: user data
static gdt_entry_t gdt[KERNEL_TSS_INDEX+1+MAX_TASK_COUNT];
static gdt_ptr_t gdt_ptr;

// Entries from index 5 on are used to store tasks' TSS (see task.c),
// starting with the initial TSS for the kernel.
gdt_entry_t *gdt_initial_tss = &gdt[KERNEL_TSS_INDEX];
gdt_entry_t *gdt_first_task_entry = &gdt[KERNEL_TSS_INDEX+1];

// Build and return a GDT entry.
// base is the base of the segment
// limit is the limit of the segment (NOTE: it's a 20-bit value)
// type is the type of segment
// s indicates whether it's a system segment
// db indicates whether it's a code/data segment or TSS, LDT or gate
// granularity indicates 1 byte or 4KB granularity
// dpl is the privilege level
static gdt_entry_t build_entry(uint32_t base, uint32_t limit, uint8_t type, uint8_t s, uint8_t db, uint8_t granularity, uint8_t dpl) {
    gdt_entry_t entry;
    // For a TSS and LDT, base is the addresse of the TSS/LDT structure
    // and limit is the size of the structure.
    entry.lim15_0 = limit & 0xffff;
    entry.base15_0 = base & 0xffff;
    entry.base23_16 = (base >> 16) & 0xff;
    entry.type = type;  // See TYPE_xxx flags
    entry.s = s;        // 1 for segments; 0 for system (TSS, LDT, gates)
    entry.dpl = dpl;    // privilege level
    entry.present = 1;  // present in memory
    entry.lim19_16 = (limit >> 16) & 0xf;
    entry.avl = 0;      // available for use
    entry.l = 0;        // should be 0 (64-bit code segment)
    entry.db = db;      // 1 for 32-bit code and data segments; 0 for system (TSS, LDT, gate)
    entry.granularity = granularity;  // granularity of the limit value: 0 = 1 byte; 1 = 4096 bytes
    entry.base31_24 = (base >> 24) & 0xff;
    return entry;
}

// Return a NULL entry.
static gdt_entry_t gdt_make_null_segment() {
    gdt_entry_t entry;
    memsetb(&entry, 0, sizeof(gdt_entry_t));
    return entry;
}

// Return a code segment specified by the base, limit and privilege level passed in arguments.
static gdt_entry_t gdt_make_code_segment(uint32_t base, uint32_t limit, uint8_t dpl) {
    return build_entry(base, limit, TYPE_CODE_EXECONLY, S_CODE_OR_DATA, DB_SEG, 1, dpl);
}

// Return a read-write data segment specified by the base, limit and privilege level passed in arguments.
static gdt_entry_t gdt_make_data_segment(uint32_t base, uint32_t limit, uint8_t dpl) {
    return build_entry(base, limit, TYPE_DATA_RW, S_CODE_OR_DATA, DB_SEG, 1, dpl);
}

// Return a TSS entry  specified by the TSS structure and privilege level passed in arguments.
// NOTE: a TSS entry can only reside in the GDT!
gdt_entry_t gdt_make_tss(tss_t *tss, uint8_t dpl) {
    return build_entry((uint32_t)tss, sizeof(tss_t)-1, TYPE_TSS, S_SYSTEM, DB_SYS, 0, dpl);
}

// Return the selector of an entry in the GDT
uint_t gdt_entry_to_selector(gdt_entry_t *entry) {
    return GDT_INDEX_TO_SELECTOR(entry - gdt);
}

// Initialize the GDT
void gdt_init() {
    gdt_ptr.limit = sizeof(gdt)-1;
    gdt_ptr.base  = (uint32_t)&gdt;

    // Entries for null segment, kernel code and kernel data segments
    gdt[0] = gdt_make_null_segment();  // null segment
    gdt[1] = gdt_make_code_segment(0x0, 0xFFFFF, DPL_KERNEL);  // kernel code segment: 4GB limit
    gdt[2] = gdt_make_data_segment(0x0, 0xFFFFF, DPL_KERNEL);  // kernel data (+stack) segment: 4GB limit
    gdt[3] = gdt_make_code_segment(0x0, 0xFFFFF, DPL_USER);    // user code segment: 4GB limit
    gdt[4] = gdt_make_data_segment(0x0, 0xFFFFF, DPL_USER);    // user data (+stack) segment: 4GB limit

    // Load the GDT
    gdt_load(&gdt_ptr);
}
