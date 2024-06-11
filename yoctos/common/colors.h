#ifndef _COLORS_H_
#define _COLORS_H_

// Convert a 8-bit color component to 5 bits
// by removing the 3 least significant bits
#define COL8TO5(x)  (x >> 3)

// Convert a 8-bit color component to 6 bits
// by removing the 2 least significant bits
#define COL8TO6(x)  (x >> 2)

// Convert a 24-bit color to a 16-bit one (R5G6B5)
#define RGB(r,g,b) ((COL8TO5(r))<<11|(COL8TO6(g))<<5|(COL8TO5(b)))

// "Broadcast" a 16-bit color into a 32-bit word
#define COLTO32(x)  ((x&0xFFFF) << 16|(x&0xFFFF))

#define BLACK        RGB(0,0,0)
#define WHITE        RGB(255,255,255)
#define RED          RGB(255,0,0)
#define BLUE         RGB(0,0,255)
#define GREEN        RGB(0,255,0)
#define YELLOW       RGB(255,255,0)
#define CYAN         RGB(0,255,255)
#define DARK_GREY    RGB(150,150,150)
#define LIGHT_GREY   RGB(220,220,220)
#define LIGHT_BLUE   RGB(150,150,220)
#define LIGHT_GREEN  RGB(150,220,150)
#define LIGHT_RED    RGB(255,200,200)

typedef struct {
    uint16_t fg;  // foreground color
    uint16_t bg;  // background color
} term_colors_t;

#endif