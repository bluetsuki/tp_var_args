#ifndef _TERM_H_
#define _TERM_H_

#include "common/types.h"
#include "common/colors.h"

extern void term_init();
extern void term_clear();

extern term_colors_t term_getcolors();
extern void term_setcolors(term_colors_t colors);
extern void term_setfgcolor(uint16_t foreground);
extern void term_setbgcolor(uint16_t background);

extern void term_setchar(char c, int x, int y, term_colors_t color);
extern void term_putc(char c);
extern void term_puts(char *s);
extern void term_printf(char *fmt, ...);

extern void term_getcursor(int *x, int *y);
extern void term_setcursor(int x, int y);

// For debugging purposes
#define PRINT_INT(integer,color) \
{ term_colors_t col = term_getcolors(); \
    term_setfgcolor(color); \
    term_printf("{%d}", integer); \
    term_setcolors(col); }

#define PRINT_STR(string,color) \
{ term_colors_t col = term_getcolors(); \
    term_setfgcolor(color); \
    term_puts(string); \
    term_setcolors(col); }

#endif
