#include "common/string.h"
#include "common/stdio.h"
#include "common/vbe_fb.h"
#include "ulibc.h"
#include "syscall.h"
#include "ld.h"

#define TAB_SIZE  4

#define BUFFER_SIZE 1024

SECTION_DATA static vbe_fb_t fb;

void sleep(uint_t ms) {
    // TODO
    // Call syscall for sleep
	syscall(4, ms, 0, 0, 0);
}

int getc() {
    int c;
    // TODO
    // Call syscall for keyb_get_key()
	syscall(2, &c, 0, 0, 0);
    return c;
}

void putc(char c) {
	// TODO
	// Call syscall for term_putc()
	syscall(8, c, 0, 0, 0);
}

void puts(char *str) {
	// TODO
	// Call syscall for term_puts()
	syscall(0, str, 0,0,0);
}

void printf(char *fmt, ...) {
	char buffer[BUFFER_SIZE];
	va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmt, args);
    va_end(args);
	puts(buffer);
}

void set_colors(term_colors_t cols) {
	// TODO
	// Call syscall for term_set_colors()
	syscall(1, cols.fg, cols.bg, 0, 0);
}

bool task_exec(char *filename, int argc, char **argv) {
	// TODO
	// Call syscall for task_exec()
	syscall(7, filename, argc, argv, 0);
}

void timer_info(uint_t *freq, uint_t *ticks) {
	// TODO
	// Call syscall for timer_info()
	syscall(3, (uint32_t)freq, (uint32_t)ticks, 0, 0);
}

void vbe_init(uint_t *width, uint_t *height){
	// TODO
	// Call syscall for vbe_fb_info()
	syscall(5, (vbe_fb_t*)&fb, 0, 0, 0);
	*width = fb.width;
	*height = fb.height;
}

void vbe_setpixel_syscall(int x, int y, uint16_t color) {
	syscall(6, x, y, color, 0);
}

void vbe_setpixel(int x, int y, uint16_t color) {
	uint16_t* pixel = (uint16_t *) fb.addr + y * fb.pitch_in_pix + x;
	*pixel = color;
}

uint_t get_ticks() {
	uint_t ticks;
	timer_info(0, &ticks);
	return ticks;
}
// TODO: implement other syscall wrappers...

void read_string(char *buf) {
    char *start = buf;
    for (;;) {
        int c = getc();
        if (c) {
            // backspace
            if (c == '\b') {
                if (buf - start > 0) {
                    putc(c);
                    buf--;
                }
            }
            // return
            else if (c == '\n') {
                break;
            }
            // tab
            else if (c == '\t') {
                c = ' ';
                for (int i = 0; i < TAB_SIZE; i++) {
                    putc(c);
                    *buf = c;
                    buf++;
                }
            }
            else {
                putc(c);
                *buf = c;
                buf++;
            }
        }
    }
    *buf = 0;
}

// Return 1 if string str starts with prefix.
// Return 0 otherwise.
int starts_with(char *prefix, char *str) {
    while (*prefix) {
        if (*str != *prefix) {
            return 0;
        }
        prefix++;
        str++;
    }
    return 1;
}

char *trim(char *line) {
    int len;

    // Remove heading spaces.
    while (*line == ' ')
        line++;

    // Remove trailing spaces.
    len = strlen(line);
    if (len > 0) {
        char *s = line + len - 1;
        int cut = 0;
        while (*s == ' ') {
            s--;
            cut = 1;
        }
        if (cut) {
            *(s + 1) = 0;
        }
    }

    return line;
}
