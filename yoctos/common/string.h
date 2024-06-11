#ifndef _STRING_COMMON_H_
#define _STRING_COMMON_H_

#include "types.h"

// Returns the length of string s.
extern uint_t strlen(const char *s);

// Copy string src into dest, copying at most count characters.
// Returns the string dest.
extern char *strncpy(char *dest, const char *src, uint_t count);

// Compare alphanumerically string s1 with s2.
// Returns:
// 0 if identical
// a value > 0 if s1 > s2
// a value < 0 if s1 < s2
extern int strcmp(const char *s1, const char *s2);

// Identical to strcmp, except that it compares up to n characters.
extern int strncmp(const char *s1, const char *s2, uint_t n);

// Returns the integer representation of string s.
// IMPORTANT: does not perform any validation on the string!
extern uint_t atoi(const char *s);

// Convert, in-place, string s to its lower case representation.
extern char *tolower(char *s);

#endif
