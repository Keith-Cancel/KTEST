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

typedef struct {
    const char* reset;
    const char* black;
    const char* red;
    const char* green;
    const char* yellow;
    const char* blue;
    const char* magenta;
    const char* cyan;
    const char* white;
    const char* gray;
    const char* l_red;
    const char* l_green;
    const char* l_yellow;
    const char* l_blue;
    const char* l_magenta;
    const char* l_cyan;
    const char* l_white;
} fgColors, bgColors;

typedef struct {
    FILE*       out;
    int         width;
    const char* reset;
    fgColors    fg;
    bgColors    bg;
} outputInfo;

int         console_init();
const char* get_reset();
const char* get_fg_color(unsigned color);
const char* get_bg_color(unsigned color);

int         console_get_width(FILE* file);
void        console_set_output_info(outputInfo* out, FILE* file);
void        console_set_fg_colors_if_tty(bgColors* fg, FILE* file);
void        console_set_bg_colors_if_tty(bgColors* fg, FILE* file);

const char* get_reset_if_tty(FILE* file);
const char* get_fg_color_if_tty(unsigned color, FILE* file);
const char* get_bg_color_if_tty(unsigned color, FILE* file);

#endif