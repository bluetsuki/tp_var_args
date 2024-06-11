#ifndef _MODULE_H_
#define _MODULE_H_

#include "common/types.h"
#include "boot/multiboot.h"

// Returns the number of detected modules.
extern uint_t modules_count();

// Returns modules' last address in use, so we can safely assume addresses above this are free.
// Returns the kernel's last address in use if no modules are detected.
extern uint32_t modules_last_address();

// Returns a module's address.
// Note: first module is at index 0.
extern void *module_first_address(uint_t index);

// Returns a module's last address in use (inclusive).
// Note: first module is at index 0.
extern void *module_last_address(uint_t index);

// Returns a module' size in bytes.
// Note: first module is at index 0.
extern uint_t module_size(uint_t index);

// Returns a module's command line.
// Returns NULL if the module's command line is empty.
// Note: first module is at index 0.
extern char* module_cmdline(uint_t index);

// Displays modules' information:
// - number of detected modules
// - for each module: address, size (in bytes), command line
extern void modules_display_info();

// Returns the address of the first module whose argument matches name.
// Returns NULL if no such module is found.
extern void *module_addr_by_name(char *name);

// Returns the size in bytes of the first module whose argument matches name.
// Returns -1 if no such module is found.
extern int module_size_by_name(char *name);

#endif
