#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#include "common/types.h"
#include "multiboot_structs.h"

// Stores the multiboot info structure.
extern void multiboot_set_info(multiboot_info_t *_mbi);

// Retrieves the multiboot info structure.
extern multiboot_info_t* multiboot_get_info();

// Retrieves the amount of available RAM in KB.
extern uint_t multiboot_get_RAM_in_KB();

#endif
