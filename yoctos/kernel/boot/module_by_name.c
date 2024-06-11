#include "module.h"
#include "common/string.h"

// Returns the index of the first module whose cmdline matches name.
// Returns -1 if no such module is found.
static int mod_by_name(char *name) {
    for (int i = 0; i < (int)modules_count(); i++) {
        if (strcmp(name, module_cmdline(i)) == 0) {
            return i;
        }
    }
    return -1;
}

extern void *module_addr_by_name(char *name) {
    int idx = mod_by_name(name);
    if (idx == -1)
        return NULL;
    return module_first_address(idx);
}

extern int module_size_by_name(char *name) {
    int idx = mod_by_name(name);
    if (idx == -1)
        return -1;
    return module_size(idx);
}
