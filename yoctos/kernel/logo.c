
#include "common/types.h"
#include "common/string.h"
#include "common/mem.h"
#include "drivers/term.h"
#include "drivers/font.h"
#include "drivers/vbe.h"
#include "drivers/timer.h"
#include "boot/module.h"
#include "logo.h"

#define WIDTH 20

static int delay;
static int column_count;

void logo_init() {
    delay = timer_get_freq()/16;
    column_count = vbe_get_fb()->width/FONT_WIDTH;
}

void logo_render() {
    static int pos[] =  {  0,  1,  2,  3,  4,  5,  6 };
    static char msg[] = { 196,'Y','o','c','t','O','S'};
    term_setchar(179, column_count-1-WIDTH-1, 0, (term_colors_t){CYAN, BLACK});
    term_setchar(179, column_count-1, 0, (term_colors_t){CYAN, BLACK});
    for (uint_t i = 0; i < sizeof(msg); i++) {
        uint16_t fg;
        if (i) fg = YELLOW;
        else fg = LIGHT_BLUE;
        term_setchar(msg[i], column_count-1-WIDTH+pos[i], 0, (term_colors_t){fg, BLACK});
        if (timer_get_ticks() % delay == 0)
            pos[i] = (pos[i] + 1) % WIDTH;
    }
}
