#ifndef _TASK_H_
#define _TASK_H_

#include "common/types.h"
#include "tss.h"
#include "mem/paging.h"
#include "drivers/term.h"

#define MAX_TASK_COUNT  8
#define MAX_ARGS 5
#define MAX_ARGS_LENGTH 50

// Virtual address (1GB) where task user code/data is mapped (i.e. application entry point)
#define TASK_VIRT_ADDR 0x40000000

// A task has these associated structures:
// - A TSS structure (tss_t) that saves its state (context)
// - A TSS descriptor present in the GDT (which points to the tss_t structure)
// - A page directory
typedef struct {
    PDE_t pagedir[PAGETABLES_IN_PD] __attribute__((aligned(4096)));  // Task page directory
    tss_t tss __attribute__((aligned(4096)));   // Context of the task (must not cross a page boundary!)
    PTE_t *page_tables[PAGES_IN_PT];    // Save pointers to page tables in order to deallocate
                                        // previously allocated frames at task termination
    bool in_use;                        // whether the task slot is in use or free
    uint_t id;                          // task id
    uint16_t tss_selector;              // selector required when switching to the task
    uint8_t kernel_stack[65536];        // kernel stack (4KB does not seem enough!)
    uint32_t virt_addr;                 // Start of the task's virtual address space
    uint32_t addr_space_size;           // Size of the task's address space in bytes
} task_t;

extern void tasks_init();
extern bool task_exec(char *filename, int argc, char **argv);

// Implemented in task_asm.s
extern void task_ltr(uint16_t tss_selector);
extern void task_switch(uint16_t tss_selector);
extern uint16_t task_get_current_sel();

extern task_t* get_task_by_selector(uint16_t tss_selector);
extern task_t** get_task_addresses();
#endif
