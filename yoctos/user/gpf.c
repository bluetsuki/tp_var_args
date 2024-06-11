#include "ulibc.h"

void main() {
    int sec = 3;
    printf("Executing software interrupt 77 (\"int 77\") in %d sec...\n", sec);
    sleep(sec*1000);
    asm volatile("int $77");
}
