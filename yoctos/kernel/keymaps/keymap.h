#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#include "common/types.h"

#define MAP_SIZE 128

// Indicate the entry must be ignored
#define KEY_IGNORE 0x0

typedef struct {
    uint16_t normal[MAP_SIZE];
    uint16_t shift[MAP_SIZE];
    uint16_t ctrl[MAP_SIZE];
    uint16_t alt[MAP_SIZE];
} keymap_t;

#endif
