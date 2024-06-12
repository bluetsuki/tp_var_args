#include "common/string.h"
#include "common/stdio.h"
#include "common/mem.h"
#include "common/keycodes.h"
#include "boot/module.h"
#include "boot/multiboot.h"
#include "drivers/vbe.h"
#include "drivers/term.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "interrupt/idt.h"
#include "mem/paging.h"
#include "mem/frame.h"
#include "mem/gdt.h"
#include "task/task.h"
#include "x86.h"

// These are defined in the linker script: kernel.ld
extern void ld_kernel_start();
extern void ld_kernel_end();
uint_t kernel_start = (uint_t)&ld_kernel_start;
uint_t kernel_end = (uint_t)&ld_kernel_end;

void kernel_main(multiboot_info_t *mbi) {
    multiboot_set_info(mbi);
    uint_t RAM_in_KB = multiboot_get_RAM_in_KB();

    gdt_init();

    // This function must be initialized first! (before using any term_xxx functions!)
    vbe_init();
    vbe_fb_t *fb = vbe_get_fb();

    paging_init(RAM_in_KB);  // must be called AFTER vbe_init()!

    term_init();
    term_printf("YoctOS started\n");
    term_printf("VBE mode %dx%d %dbpp initialized (addr=0x%x, pitch=%d).\n", fb->width, fb->height, fb->bpp, fb->addr, fb->pitch_in_bytes);
    term_printf("Detected %dKB of RAM.\n", RAM_in_KB);
    term_printf("%dKB of RAM available.\n", frame_total_free()*FRAME_SIZE/1024);
    term_printf("Kernel loaded at [0x%x-0x%x], size=%dKB\n", kernel_start, kernel_end, (kernel_end-kernel_start)/1024);

    modules_display_info();

    pic_init();
    idt_init();
    keyb_init();
	tasks_init();

    // IMPORTANT: timer frequency must be >= 50
    int timer_freq = 1000;
    timer_init(timer_freq);

    // Unmask hardware interrupts
    sti();
    term_puts("Interrupts enabled.\n");

	task_exec("shell.exe", 0, NULL);

    term_printf("\nSystem halted.");
    halt();
}
