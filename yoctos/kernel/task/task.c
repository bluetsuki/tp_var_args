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
static task_t *task_create(char *name, uint_t addr_space_size, int args_size, int argc, char **argv) {
    // Tasks GDT entries start at gdt_first_task_entry (each task uses one GDT entry)
    extern gdt_entry_t *gdt_first_task_entry;
    gdt_entry_t *gdt_task_tss = gdt_first_task_entry;

    
    int total_mem_size = (TASK_STACK_SIZE_MB * 1024 * 1024) + args_size + addr_space_size;

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
    // aggrandir l'esapce d'addr pour les args
	t->addr_space_size = addr_space_size + args_size;

    for (uint_t i = 0; i < PAGETABLES_IN_PD; i++)
    {
        t->pagedir[i] = pagedir_templ[i];
    }
    tss_t *tss = &t->tss;

    tss->cs = GDT_USER_CODE_SELECTOR;
    tss->ds = tss->es = tss->gs = tss->ss = GDT_USER_DATA_SELECTOR;
    tss->cr3 = (uint32_t)t->pagedir;
    tss->eflags = (1 << 9);
    tss->ss0 = GDT_KERNEL_DATA_SELECTOR;
    tss->esp0 = (uint32_t)(t->kernel_stack) + sizeof(t->kernel_stack);
    tss->eip = t->virt_addr;
    // tss->esp = tss->ebp = t->virt_addr + addr_space_size + (TASK_STACK_SIZE_MB << 20);
    tss->esp = tss->ebp = t->virt_addr + total_mem_size;

    uint_t alloc_frame_count = 0;
	uint_t id = t->id;
    alloc_frame_count = paging_alloc(t->pagedir, t->page_tables, t->virt_addr, t->addr_space_size + (TASK_STACK_SIZE_MB << 20) + args_size, PRIVILEGE_USER);

    
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

    void* module_addr = module_addr_by_name(filename);

    // Calculer la taille des arguments
    uint_t args_size = sizeof(int);
    for (int i = 0; i < argc; i++) {
        args_size += (strlen(argv[i]) * sizeof(char)) + 1;
    }
    args_size += (argc + 1) * sizeof(char *); // Espace pour les pointeurs argv

    // copie de la mémoire avant le changement de pagination
    char tmp_argv[MAX_ARGS][MAX_ARGS_LENGTH];
    for (int i = 0; i < argc; i++) {
        int len = strlen(argv[i]) + 1;
        strncpy(tmp_argv[i], argv[i], len);
        tmp_argv[i][len] = '\0';
    }

    if (module_addr == NULL) {
        term_printf("Failed to load binary \"%s\".\n", filename);
        return NULL;
    }
     
    uint_t mod_size = module_size_by_name(filename);
    if (mod_size == -1) {
        term_printf("Empty binary \"%s\".\n", filename);
        return NULL;
    }

    t = task_create(filename, mod_size, args_size, argc, argv);
    if (!t) {
        return NULL;
    }

    PDE_t* pagedir = paging_get_current_pagedir();
	paging_load_pagedir(t->pagedir);
	memcpy((void*)t->virt_addr, module_addr, mod_size);

    // copie argc apres le module
    memcpy((void*)t->virt_addr + mod_size, &argc, sizeof(argc));
    int offset = 0;
    for (int i = 0; i < argc; i++) {
        int args_len = strlen(tmp_argv[i]) + 1;
        memcpy((void*)t->virt_addr + mod_size + sizeof(int) + offset, tmp_argv[i], args_len); //Faut les faire un par un
        offset += args_len;
    }

	paging_load_pagedir(pagedir);
    return t;
}


// Loads a task and executes it.
// Returns false if it failed.
bool task_exec(char *filename, int argc, char **argv) {
    task_t *t = task_load(filename,  argc, argv);
    if (!t) {
        return false;
    }
    term_colors_t cols = term_getcolors();
    task_switch(t->tss_selector);
    term_setcolors(cols);
    task_free(t);
    return true;
}
