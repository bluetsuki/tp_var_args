#include "boot/multiboot.h"
#include "boot/module.h"
#include "common/mem.h"
#include "common/string.h"
#include "descriptors.h"
#include "mem/gdt.h"
#include "mem/frame.h"
#include "drivers/term.h"
#include "drivers/vbe.h"
#include "task.h"
#include "x86.h"
#include "tss.h"
#include "mem/paging.h"

#define TASK_STACK_SIZE_MB 2

static task_t tasks[MAX_TASK_COUNT];
static tss_t initial_tss;
static uint8_t initial_tss_kernel_stack[65536];
static uint_t task_id = 1;  // incremented whenever a new task is created

// Template page directory for all tasks.
// Since it will never be loaded as a page directory, there is no need to align it to 4KB.
static PDE_t pagedir_templ[PAGETABLES_IN_PD];

// Creates and returns a task from the fixed pool of tasks.
// This function dynamically allocates the address space of size "size" (in bytes) for the task.
// Returns NULL if it failed.
static task_t *task_create(char *name, uint_t addr_space_size, int arg_mem_size, int argc, char **argv) {
    // Tasks GDT entries start at gdt_first_task_entry (each task uses one GDT entry)
    extern gdt_entry_t *gdt_first_task_entry;
    gdt_entry_t *gdt_task_tss = gdt_first_task_entry;

    uint_t total_mem_size = addr_space_size + (TASK_STACK_SIZE_MB << 20) + arg_mem_size;

    // TODO

    // Look for a free task and if found:
    // - initializes the task's fields
    // - initializes its GDT entry and TSS selector
    // - initializes its TSS structure
    // - creates its RAM and VBE identity mappings by using the common template page directory
    // - allocates its address space using the "paging_alloc" function

    // Variables declared here to prevent compilation error 
    task_t *t = NULL;

	for (uint_t i = 0; i < MAX_TASK_COUNT; i++) {
        if (!(tasks[i].in_use)) {
            t = &tasks[i];
            memset(t, 0, sizeof(task_t));
            task_id++; 
			t->in_use = true;
			t->id = i;
            break;
        }
        gdt_task_tss++;
    }

    if (t == NULL)
        return NULL;

    int gdt_tss_sel = gdt_entry_to_selector(gdt_task_tss);
    *gdt_task_tss = gdt_make_tss(&t->tss, DPL_KERNEL);
    t->tss_selector = gdt_tss_sel;
	
	t->virt_addr = TASK_VIRT_ADDR;
	t->addr_space_size = addr_space_size;

    for (uint_t i = 0; i < PAGETABLES_IN_PD; i++)
    {
        t->pagedir[i] = pagedir_templ[i];
    }

    // Alloue l'espace d'adressage de la tâche

    // Stocke `argc` et `argv` à la fin de l'espace d'adressage de la tâche
    uint_t arg_mem_start = t->virt_addr + total_mem_size - arg_mem_size;
    uint_t arg_mem_ptr = arg_mem_start;

    *(int *)arg_mem_ptr = argc;
    arg_mem_ptr += sizeof(int);

    char **argv_ptr = (char **)arg_mem_ptr;
    arg_mem_ptr += argc * sizeof(char *);

    for (int i = 0; i < argc; i++) {
        argv_ptr[i] = (char *)arg_mem_ptr;
        strncpy((char *)arg_mem_ptr, argv[i], argc);
        arg_mem_ptr += strlen(argv[i]) + 1;
    }

    tss_t *tss = &t->tss;

    tss->cs = GDT_USER_CODE_SELECTOR;
    tss->ds = tss->es = tss->gs = tss->ss = GDT_USER_DATA_SELECTOR;
    tss->cr3 = (uint32_t)t->pagedir;
    tss->eflags = (1 << 9);
    tss->ss0 = GDT_KERNEL_DATA_SELECTOR;
    tss->esp0 = (uint32_t)(t->kernel_stack) + sizeof(t->kernel_stack);
    tss->eip = arg_mem_size;
    tss->esp = tss->ebp = t->virt_addr + arg_mem_size;
    // tss->eip = t->virt_addr;
    // tss->esp = tss->ebp = t->virt_addr + addr_space_size + (TASK_STACK_SIZE_MB << 20);
    tss->eax = arg_mem_start; // Passe l'adresse de `argc` à la tâche
    uint_t alloc_frame_count = 0;
	uint_t id = t->id;
    // alloc_frame_count = paging_alloc(t->pagedir, t->page_tables, t->virt_addr, t->addr_space_size + (TASK_STACK_SIZE_MB << 20), PRIVILEGE_USER);
    alloc_frame_count = paging_alloc(t->pagedir, t->page_tables, t->virt_addr, total_mem_size, PRIVILEGE_USER);

    // TODO
    // Look for a free task and if found:
    // - initializes the task's fields
    // - initializes its GDT entry and TSS selector
    // - initializes its TSS structure
    // - creates its RAM and VBE identity mappings by using the common template page directory
    // - allocates its address space using the "paging_alloc" function

    // Variables declared here to prevent compilation error
    term_printf("Allocated %dKB of RAM for task %d (\"%s\")\n", alloc_frame_count * PAGE_SIZE / 1024, id, name);

    return t;
}

task_t* get_task_by_selector(uint16_t selector){
	extern gdt_entry_t *gdt_first_task_entry;
    gdt_entry_t *gdt_task_tss = gdt_first_task_entry;
	
	for (uint_t i = 0; i < MAX_TASK_COUNT; i++) {
		int gdt_tss_sel = gdt_entry_to_selector(gdt_task_tss);
		if(selector == gdt_tss_sel){
			return &tasks[i];	
			break;
		}
		gdt_task_tss++;
    }	
	return NULL;
}
 
// Frees a task previously created with task_create().
// This function frees the task's page frames.
static void task_free(task_t *t) {
    // Make sure to free:
    // - every frame allocated for a page
    // - every frame allocated for a page table
    // Note: page.present indicates if a page was mapped

    // Variables declared here to prevent compilation error
    uint_t alloc_frame_count = 0;
    uint_t alloc_pt_count = 0;

    // Iterates until reachying a NULL pointer indicating that
    // there is no more allocated page table
    for (uint_t pt = 0; t->page_tables[pt]; pt++) {
      	PTE_t *page_table = (PTE_t *) t->page_tables[pt];
        if (t->page_tables[pt]->present == 1) {
            for (uint32_t i = 0; i < PAGES_IN_PT; i++) {
				if (page_table[i].present == 1) {
					void *addr = FRAME_NB_TO_ADDR(page_table[i].frame_number);
					frame_free(addr);
				}
            }
            alloc_frame_count += PAGES_IN_PT;
            alloc_pt_count++;
            t->page_tables[pt]->present = 0;
            frame_free(FRAME_NB_TO_ADDR(page_table->frame_number));
        }
    }

	task_id -= 1;
	t->in_use = false;

    term_printf("Freed %dKB of RAM (%d page table(s), %d frames)\n",
                (alloc_frame_count)*PAGE_SIZE/1024,
                alloc_pt_count, alloc_frame_count);
}

// Initializes the task subsystem
void tasks_init() {
    memset(tasks, 0, sizeof(tasks));

    // Allocates and initializes the initial TSS: it is where the CPU state
    // will be saved before switching from the kernel to the very first user task.
    extern gdt_entry_t *gdt_initial_tss;
    *gdt_initial_tss = gdt_make_tss(&initial_tss, DPL_KERNEL);
    memset(&initial_tss, 0, sizeof(tss_t));
    initial_tss.ss0 = GDT_KERNEL_DATA_SELECTOR;
    initial_tss.esp0 = ((uint32_t)initial_tss_kernel_stack) + sizeof(initial_tss_kernel_stack);
    initial_tss.cr3 = (uint32_t)paging_get_current_pagedir();

    // TODO
	// Identity maps available RAM so kernel code can access its own code / data
    // Creates a common template page directory (pagedir_templ) that will be shared by each task.
    // This mapping:
    // - identity maps the available RAM so the kernel can access it during a syscall as if there
    //   were no paging
    // - identity maps the VBE framebuffer so that tasks can access it without requiring syscalls

	paging_mmap(pagedir_templ, 0, 0, multiboot_get_RAM_in_KB () *1024, PRIVILEGE_KERNEL, ACCESS_READWRITE);

	vbe_fb_t *fb = vbe_get_fb();
	paging_mmap(pagedir_templ, fb->addr, fb->addr, fb->size, PRIVILEGE_USER, ACCESS_READWRITE);

    // Loads the task register to point to the initial TSS selector.
    // IMPORTANT: The GDT must already be loaded before loading the task register!
    task_ltr(gdt_entry_to_selector(gdt_initial_tss));

    term_puts("Tasks initialized.\n");
}

// Creates a new task with the content of the specified binary application.
// Once loaded, the task is ready to be executed.
// Returns NULL if it failed (ie. reached max number of tasks).
static task_t *task_load(char *filename, int argc, char **argv) {

	task_t* t = NULL;

    // Allocation mémoire supplémentaire pour `argc` et `argv`
    uint_t arg_mem_size = sizeof(int) + argc * sizeof(char *) + sizeof(char) * 256; // Estimation taille totale pour `argc` et `argv`
    
	void* module_addr = module_addr_by_name(filename);
	if (module_addr == NULL) {
		term_printf("Failed to load binary \"%s\".\n", filename);
		return NULL;
	}
	 
	uint_t size = module_size_by_name(filename);
	if (size == -1) {
		term_printf("Empty binary \"%s\".\n", filename);
		return NULL;
	}

	t = task_create(filename, size, arg_mem_size, argc, argv);
	if (!t) {
		return NULL;
	}

	PDE_t* pagedir = paging_get_current_pagedir();
	paging_load_pagedir(t->pagedir);
	memcpy((void*)t->virt_addr, module_addr, size + arg_mem_size);
	paging_load_pagedir(pagedir);
    // - Create a new task using the "task_create" function.
    // - Temporarily maps the new task's address space (page directory) to allow the current task to
    // contiguously write the binary into the new task's address space (make sure to save the current
    // mapping (page directory) before doing so and restoring it once done).
    // - IMPORTANT: beware of pointers pointing to addresses in the current task!
    // Their content won't be reachable during the temporary mapping of the new task.
    // Thus, make sure to either reference them before the mapping (or copy their content before hand).

    return t;
}

// Loads a task and executes it.
// Returns false if it failed.
bool task_exec(char *filename, int argc, char **argv) {
    task_t *t = task_load(filename, argc, argv);
    if (!t) {
        return false;
    }
    term_colors_t cols = term_getcolors();
    task_switch(t->tss_selector);
    term_setcolors(cols);
    task_free(t);
    return true;
}
