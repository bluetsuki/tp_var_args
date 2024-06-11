#ifndef _VBE_FB_COMMON_H_
#define _VBE_FB_COMMON_H_

#include "common/types.h"

typedef struct {
    uint16_t *addr;
    uint32_t size;  // in bytes
    uint32_t pitch_in_bytes;
    uint32_t pitch_in_pix;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
} vbe_fb_t;

#endif
