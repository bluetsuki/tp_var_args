#include <stdbool.h>
#include "stdio.h"
#include "types.h"

// Print into buf in reverse, the value val in the given base.
// The argument sign indicates whether the value is signed (true) or not (false).
// The number of characters written is returned into length.
static void printint_rev(char *buf, int *length, int val, int base, bool sign) {
    static char digits[] = "0123456789ABCDEF";
    int count = 0;
    uint_t x;

    if (sign && (sign = val < 0))
        x = -val;
    else
        x = val;

    do {
        buf[count++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[count++] = '-';

    buf[count] = 0;
    *length = count;
}

int snprintf(char *dest, int size, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int count = vsnprintf(dest, size, fmt, args);
    va_end(args);
    return count;
}

// Helper macro used in vsnprintf
#define putc(c)     \
    if (size > 0) { \
        *dest = c;  \
        dest++;     \
        size--;     \
        count++;    \
    }

int vsnprintf(char *dest, int size, const char *fmt, va_list args) {
    char buf[size+1];
    int len;
    int count = 0;
    while (*fmt) {
        if (*fmt == '%') {  // found an argument
            fmt++;
            if (*fmt == 0)  // missing value after %
                break;
            switch (*fmt) {
                case 'c':
                    {
                        int c = va_arg(args, int);
                        putc(c)
                    }
                    break;
                case 's':
                    {
                        char *s = va_arg(args, char *);
                        while (*s) {
                            putc(*s)
                            s++;
                        }
                    }
                    break;
                case 'd':
                    {
                        int n = va_arg(args, int);
                        printint_rev(buf, &len, n, 10, true);
                        while (len) putc(buf[--len]);
                    }
                    break;
                case 'u':
                    {
                        uint_t n = va_arg(args, uint_t);
                        printint_rev(buf, &len, n, 10, false);
                        while (len) putc(buf[--len]);
                    }
                    break;
                case 'x':
                    {
                        int n = va_arg(args, int);
                        printint_rev(buf, &len, n, 16, false);
                        while (len) putc(buf[--len]);
                    }
                    break;
                case '%':
                    putc('%')
                    break;
                default:
                    // unkown argument, ignore it!
                    break;
            }
        }
        else {
            putc(*fmt)
        }
        fmt++;
    }

    *dest = 0;
    return count;
}
