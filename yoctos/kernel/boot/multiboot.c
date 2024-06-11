#include "multiboot.h"

static multiboot_info_t *mbi;

multiboot_info_t* multiboot_get_info() {
    return mbi;
}

void multiboot_set_info(multiboot_info_t *_mbi) {
    mbi = _mbi;
}

uint_t multiboot_get_RAM_in_KB() {
    return mbi->mem_lower + mbi->mem_upper;
}

