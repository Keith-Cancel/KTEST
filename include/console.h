#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>

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

const char* get_reset_if_tty(FILE* file);
const char* get_fg_color_if_tty(unsigned color, FILE* file);
const char* get_bg_color_if_tty(unsigned color, FILE* file);

#endif