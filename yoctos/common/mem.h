#ifndef _MEM_COMMON_H_
#define _MEM_COMMON_H_

#include "types.h"

#define memsetb memset
#define memcpyb memcpy

// Fills count bytes of the memory area pointed to by dst with the byte value.
extern void memset(void *dst, uint8_t value, uint_t count);

// Fills count dwords of the memory area pointed to by dst with the dword value.
// NOTE: dword = double word = 32-bit
extern void memsetdw(void *dst, uint32_t value, uint_t count);

// Copy count bytes from src to dst.
extern void memcpy(void *dst, void *src, uint_t count);

// Copy count dwords from src to dst.
// NOTE: dword = double word = 32-bit
extern void memcpydw(void *dst, void *src, uint_t count);

#endif
