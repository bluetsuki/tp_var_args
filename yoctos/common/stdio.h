#ifndef _STDIO_COMMON_H_
#define _STDIO_COMMON_H_

#include <stdarg.h>

// Writes at most size characters (including terminal 0) of the formatted string fmt
// into buffer dest.
// args are the variable length arguments as a va_list.
// Returns the number of characters written.
extern int vsnprintf(char *dest, int size, const char *fmt, va_list args);

// Writes at most size characters (including terminal 0) of the formatted string fmt
// into buffer dest.
// Any arbitrary number of arguments can be passed after fmt.
// Returns the number of characters written.
extern int snprintf(char *dest, int size, char *fmt, ...);

#endif
