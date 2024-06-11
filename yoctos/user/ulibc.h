#ifndef _ULIBC_H_
#define _ULIBC_H_

#include "common/types.h"
#include "common/colors.h"
#include "common/string.h"
#include "common/vbe_fb.h"

extern bool task_exec(char *filename, int argc, char **argv);
extern void exit();  // defined in entrypoint_asm.s

extern void timer_info(uint_t *freq, uint_t *ticks);
extern uint_t get_ticks();
extern void sleep(uint_t ms);

extern void vbe_init(uint_t *width, uint_t *height);
extern void vbe_setpixel(int x, int y, uint16_t color);
extern void vbe_setpixel_syscall(int x, int y, uint16_t color);

extern void read_string(char *buf);
extern int getc();

extern int starts_with(char *prefix, char *str);
extern char *trim(char *line);

extern void putc(char c);
extern void puts(char *str);
extern void printf(char *fmt, ...);
extern void set_colors(term_colors_t cols);

#endif
