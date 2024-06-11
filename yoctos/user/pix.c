#include "ulibc.h"
#include "common/vbe_fb.h"

// For this performance measurment to be meaningful, think of compiling YoctOS
// with "make clean && make run DEBUG=0" which uses compiler optimizations!

void main() {
	uint_t width, height;
	vbe_init(&width, &height);
	int nb_loops = 500;
	
	// TODO
	// Fill the whole screen many times and measure the time it takes by:
	// - using the syscall implementation of setpixel
	// - using the userspace setpixel implementation (ie. without syscalls

	uint_t start_1 = get_ticks();

	// Clear the screen nb_loops times widthout syscalls
	for (int x = 0; x < nb_loops; x++) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				vbe_setpixel(i, j, 0x0);
			}
		}
	}

	uint_t end_1 = get_ticks();

	uint_t start_2 = get_ticks();
	// Clear the screen nb_loops times with syscalls
	for (int x = 0; x < nb_loops; x++) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				vbe_setpixel_syscall(i, j, 0x0);
			}
		}
	}
	uint_t end_2 = get_ticks();

	printf("Without syscalls: %d ticks\n", end_1 - start_1);
	printf("With syscalls: %d ticks\n", end_2 - start_2);
}
