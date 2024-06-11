#include <stdint.h>
#include "mem.h"

void memset(void *dst, uint8_t value, uint_t count) {
    uint8_t *d = dst;
    while (count--)
        *d++ = value;
}

void memsetdw(void *dst, uint32_t value, uint_t count) {
    uint32_t *d = dst;
    while (count--)
        *d++ = value;
}

void memcpy(void *dst, void *src, uint_t count) {
    uint8_t *d = dst;
    uint8_t *s = src;
    while (count--)
        *d++ = *s++;
}

void memcpydw(void *dst, void *src, uint_t count) {
    uint32_t *d = dst;
    uint32_t *s = src;
    while (count--)
        *d++ = *s++;
}
