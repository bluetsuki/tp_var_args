#ifndef _PAGING_H_
#define _PAGING_H_

#include "common/types.h"

// The hardware supports 3 page sizes: 4KB, 4MB and 2MB (when PAE is enabled)
// Our kernel only uses 4KB pages.
#define PAGE_SIZE  4096

// The maximum number of pages in total on a IA-32 architecture is 2^20 (= 1048576 pages).
// 1048576 * 4096KB = 4GB

// Number of pages in a page table
#define PAGES_IN_PT 1024

// Number of page tables in a page directory
// (number of page directory entries in a page directory)
#define PAGETABLES_IN_PD 1024

// Returns the index of the PDE (page directory entry) for a given virtual address
#define ADDR_TO_PDE(addr) (((uint32_t)addr) >> 22)

// Converts a 32-bit address into a page number
#define ADDR_TO_PAGE_NB(addr) (((uint32_t)addr)/PAGE_SIZE)

// Converts a page number into a 32-bit address
#define PAGE_NB_TO_ADDR(n) (((uint32_t)n)*PAGE_SIZE)

// Returns the number of pages required to store the given number of bytes
#define PAGE_COUNT(size) ((size + PAGE_SIZE - 1)/PAGE_SIZE)

// For details about a page directories and a page tables, see Vol. 3A System Programming Guide Part 1
// of the Intel 64 and IA-32 Architecture Software Dev's Manual.
// In particular, section 3.7.6 Page-Directory and Page-Table Entries

enum privilege_t {
    PRIVILEGE_KERNEL = 0,
    PRIVILEGE_USER = 1
};

enum access_t {
    ACCESS_READONLY = 0,
    ACCESS_READWRITE = 1
};

// Structure of a page directory entry (PDE). A page directory is an array of page directory
// entries.
// A page table can hold 1024 pages which means addressing up to 4MB of RAM (using 4KB pages).
// Using 1024 page tables, we can address 4GB of RAM.
typedef struct {
    // The bit below is the least significant
    uint32_t present : 1;        // page table currently loaded in memory (used for swapping)
    uint32_t rw : 1;             // specifies read-write privileges
    uint32_t user : 1;           // specifies user or supervisor privilege
    uint32_t write_through : 1;  // enable/disable write-through caching for the associated
                                 // page table
    uint32_t cache_disable : 1;  // enable/disable caching of the associated page table
                                 // (useful for MMIO)
    uint32_t accessed : 1;       // page table has been accessed by read or write (set by cpu)
    uint32_t reserved : 1;       // reserved (0)
    uint32_t page_sz : 1;        // page size: 0 = 4KB, 1 = 4MB
    uint32_t gp : 1;             // indicates a global page (ignored)
    uint32_t available : 3;      // unused, freely available for kernel use
    // Frame number of the PT this PDE points to.
    // To get the PT's physical address, simply multiply this by page size
	uint32_t pagetable_frame_number : 20;
    // The 20 bits above are the most significant
} __attribute__((packed)) PDE_t;

// Structure of a page table entry (PTE). A page table is an array of page table entries.
// A page can hold 2^12 bytes = 4096 bytes = 4KB.
typedef struct {
    // The bit below is the least significant
    uint32_t present : 1;        // page currently loaded in memory (used for swapping)
    uint32_t rw : 1;             // specifies read-write privileges
    uint32_t user : 1;           // specifies user or supervisor privilege
    uint32_t write_through : 1;  // enable/disable write-through caching for the associated page
    uint32_t cache_disable : 1;  // enable/disable caching of the associated page
                                 // (useful for MMIO)
    uint32_t accessed : 1;       // page has been accessed by read or write (set by cpu)
    uint32_t dirty : 1;          // page has been written to (set by cpu)
    uint32_t pat : 1;            // only used when using "Page Attribute Table" (PAT),
                                 // 0 otherwise
    uint32_t gp : 1;             // indicates a global page
    uint32_t available : 3;      // unused, freely available for kernel use
    uint32_t frame_number : 20;  // frame number the page points to
    // The 20 bits above are the most significant
} __attribute__((packed)) PTE_t;

// Setup the kernel page directory with the following two mappings:
// - Identity map the available RAM so that the kernel can access it as if there was no paging.
// - Identity map the VBE framebuffer.
// Then, loads the page directory and activate paging.
extern void paging_init(uint_t RAM_in_KB);

// Maps, in the specified page directory, size bytes starting at virtual address virt_addr
// into physical address phys_addr. Both virtual and physical areas are contiguous.
// This function dynamically allocates the necessary frames to store the page tables.
// IMPORTANT: virt_addr and phys_addr must be aligned to a page size (4KB).
extern void paging_mmap(PDE_t *pagedir, uint32_t virt_addr, uint32_t phys_addr, uint32_t size, enum privilege_t privilege, enum access_t access);

// Enable paging.
// Assembly function implemented in paging_asm.s
extern void paging_enable();

// Load the specified page directory into the CPU and make it active.
// Assembly function implemented in paging_asm.s
extern void paging_load_pagedir(PDE_t *pagedir);

// Return the page directory currently used by the CPU.
// Assembly function implemented in paging_asm.s
extern PDE_t *paging_get_current_pagedir();

// Allocates size bytes of physical memory (as frames, so not necessarily contiguous), then maps these
// frames at virtual address virt_addr. The area at virt_addr is guaranteed to be contiguous.
// The mapping is performed in the specified page directory.
// This function dynamically allocates the necessary frames to store the page tables and stores
// their addresses into page_tables so that they can be subsequently freed.
// If NULL is passed in page_tables, then the allocated frames' addresses won't be stored.
// IMPORTANT: virt_addr and phys_addr must be aligned to a page size (4KB).
// Returns the number of frames allocated (including frames used to store page tables).
extern uint_t paging_alloc(PDE_t *pagedir, PTE_t *page_tables[PAGETABLES_IN_PD], uint32_t virt_addr, uint32_t size, enum privilege_t privilege);

#endif
