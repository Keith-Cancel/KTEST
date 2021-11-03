#ifndef CONSOLE_H
#define CONSOLE_H

enum colors {
    RESET = 0,
    BLACK,  RED,       GREEN,   YELLOW,
    BLUE,   MAGENTA,   CYAN,    WHITE,
    GRAY,   L_RED,     L_GREEN, L_YELLOW,
    L_BLUE, L_MAGENTA, L_CYAN,  L_WHITE
};

int         console_init();
const char* get_reset();
const char* get_fg_color(unsigned color);
const char* get_bg_color(unsigned color);

#endif