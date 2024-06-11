#ifndef _VBE_H_
#define _VBE_H_

#include "common/types.h"
#include "common/vbe_fb.h"

extern void vbe_init();
extern vbe_fb_t *vbe_get_fb();
extern void vbe_setpixel(int x, int y, uint16_t color);
extern uint16_t vbe_getpixel(int x, int y);
extern void vbe_clear(uint16_t color);

#endif
