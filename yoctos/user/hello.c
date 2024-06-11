#include "user/ulibc.h"

void main() {
    term_colors_t cols = { .fg = YELLOW, .bg = RED };
    set_colors(cols);
	printf("=================\n");
	printf(" HELLO WORLD !!! \n");
	printf("=================\n");
}
