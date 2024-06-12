#include "boot/multiboot.h"
#include "common/types.h"
#include "mem/gdt.h"
#include "task/task.h"
#include "mem/frame.h"
#include "mem/paging.h"
#include "drivers/term.h"
#include "drivers/vbe.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "syscall.h"
#include "x86.h"

static int syscall_term_puts(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
    UNUSED(arg4);
	term_puts((char *)(arg1));
    return 0;
}

static int syscall_term_set_colors(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg3);
	UNUSED(arg4);
	term_colors_t term = { .fg = (uint16_t) arg1, .bg = (uint16_t) arg2 };
	term_setcolors(term);
	return 0;
} 

static int syscall_keyb_get_key(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	int val = keyb_get_key();
	*((int*) arg1) = val;
	return 0;
}

static int syscall_timer_info(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg3);
	UNUSED(arg4);
	*((uint_t*) arg1) = timer_get_freq();
	*((uint_t*) arg2) = timer_get_ticks();
    return 0;
}

static int syscall_timer_sleep(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	timer_sleep((uint_t) arg1);
	return 0;
}

static int syscall_vbe_fb_info(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	vbe_fb_t* vb = (vbe_fb_t*) arg1;
	*vb = *vbe_get_fb();
	return 0;
} 

static int syscall_vbe_setpix(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg4);
	vbe_setpixel((int) arg1, (int) arg2, (uint16_t) arg3);
}

static int syscall_task_exec(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg4);
	task_exec((char *) arg1, (int)arg2, (char**)arg3);
	return 0;
}

static int syscall_putc(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	term_putc((uint8_t)arg1);
	return 0;
}

static int syscall_mod_size(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg3);
	UNUSED(arg4);
	char *filename = (char*)arg2;
	*filename = module_size_by_name((char*)arg1);
	return 0;
}

static int syscall_task_addr_by_id(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	UNUSED(arg2);
	UNUSED(arg3);
	UNUSED(arg4);
	void* t = (void *)arg2;
	t = get_task_addr_by_id((int)arg1);
	return 0;
}

// Map syscall numbers to functions
static int (*syscall_func[])(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) = {
    syscall_term_puts,
	syscall_term_set_colors,
	syscall_keyb_get_key,
	syscall_timer_info,
	syscall_timer_sleep,
	syscall_vbe_fb_info,
	syscall_vbe_setpix,
	syscall_task_exec,
	syscall_putc,
	syscall_mod_size,
	syscall_task_addr_by_id
};

// Called by the assembly function: _syscall_handler
// Call the syscall number nb.
// Returns the value returned by the syscall function or -1 if nb was invalid.
int syscall_handler(int nb, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
	if (nb >= 0 && nb < 9) {
		return syscall_func[nb](arg1, arg2, arg3, arg4);
	} else {
		return -1;
	}
}
