#include <stdint.h>
#include "string.h"

uint_t strlen(const char *s) {
    int n;
    for (n = 0; s[n]; n++);
    return n;
}

char *strncpy(char *dest, const char *src, uint_t count) {
    char *p = dest;

    while (count && *src) {
        *p = *src;
        src++;
        p++;
        count--;
    }
    *p = 0;
    return dest;
}

// Original FreeBSD implementation
// Source: https://github.com/freebsd/freebsd/blob/master/lib/libc/string/strcmp.c
int strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2++)
        if (*s1++ == '\0')
            return (0);
    return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}

// Original FreeBSD implementation
// Source: https://github.com/freebsd/freebsd/blob/master/lib/libc/string/strncmp.c
int strncmp(const char *s1, const char *s2, uint_t n) {
    if (n == 0)
        return (0);
    do {
        if (*s1 != *s2++)
            return (*(const unsigned char *)s1 -
                *(const unsigned char *)(s2 - 1));
        if (*s1++ == '\0')
            break;
    } while (--n != 0);
    return (0);
}

uint_t atoi(const char *s) {
    uint_t n = 0;
    while ('0' <= *s && *s <= '9')
        n = n*10 + *s++ - '0';
    return n;
}

char *tolower(char *s) {
    for (int n = 0; s[n]; n++) {
        if (s[n] >= 'A' && s[n] <= 'Z') {
            // upper case letters differ by 32 from lower case ones (man ascii)
            s[n] = s[n] + 32;
        }
    }
    return s;
}
