#define _XOPEN_SOURCE 700

#include "console.h"
#include "sys-info.h"

#include <locale.h>

#if CURRENT_OS == OS_WINDOWS
    #include <windows.h>
    #include <io.h>
    #define isatty _isatty
    #define fileno _fileno
#elif CURRENT_OS == OS_LINUX || CURRENT_OS == OS_UNIX_LIKE
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif


// ANSI Escape Sequences
static const char* fg_colors[17] = {
    "\x1b[0m",
    "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m",
    "\x1b[90m", "\x1b[91m", "\x1b[92m", "\x1b[93m",
    "\x1b[94m", "\x1b[95m", "\x1b[96m", "\x1b[97m"
};

// ANSI Escape Sequences
static const char* bg_colors[17] = {
    "\x1b[0m",
    "\x1b[40m",  "\x1b[41m",  "\x1b[42m",  "\x1b[43m",
    "\x1b[44m",  "\x1b[45m",  "\x1b[46m",  "\x1b[47m",
    "\x1b[100m", "\x1b[101m", "\x1b[102m", "\x1b[103m",
    "\x1b[104m", "\x1b[105m", "\x1b[106m", "\x1b[107m"
};

static const char* empty_str = "";

int console_init() {
    setlocale(LC_ALL, "en_US.UTF-8");
    #if CURRENT_OS == OS_WINDOWS
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD  mode;
        GetConsoleMode(console_out, &mode);
        mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if(!SetConsoleMode(console_out, mode)) {
            return 1;
        }
    #endif
    return 0;
}

const char* get_fg_color(unsigned color) {
    if(color > L_WHITE) {
        return fg_colors[RESET];
    }
    return fg_colors[color];
}

const char* get_bg_color(unsigned color) {
    if(color > L_WHITE) {
        return bg_colors[RESET];
    }
    return bg_colors[color];
}

const char* get_reset() {
    return fg_colors[0];
}

const char* get_reset_if_tty(FILE* file) {
    int fd = fileno(file);
    if(fd == -1 || !isatty(fd)) {
        return empty_str;
    }
    return get_reset();
}

const char* get_fg_color_if_tty(unsigned color, FILE* file) {
    int fd = fileno(file);
    if(fd == -1 || !isatty(fd)) {
        return empty_str;
    }
    return get_fg_color(color);
}

const char* get_bg_color_if_tty(unsigned color, FILE* file) {
    int fd = fileno(file);
    if(fd == -1 || !isatty(fd)) {
        return empty_str;
    }
    return get_bg_color(color);
}

int console_get_width(FILE* file) {
    int fd = fileno(file);
    if(fd == -1 && !isatty(fd)) {
        return -1;
    }
    #if CURRENT_OS == OS_WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO info = { 0 };
        intptr_t ptr = _get_osfhandle(fd);
        HANDLE h = (HANDLE)ptr;
        if(h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        if(GetConsoleScreenBufferInfo(h, &info) == 0) {
            return -1;
        }
        return (info.srWindow.Right - info.srWindow.Left);
    #elif CURRENT_OS == OS_LINUX || CURRENT_OS == OS_UNIX_LIKE
        struct winsize sz = { 0 };
        if(ioctl(fd, TIOCGWINSZ, &sz) == -1) {
            return -1;
        }
        return (int)(sz.ws_col);
    #endif
}

void console_set_fg_colors_if_tty(bgColors* fg, FILE* file) {
    fg->reset     = get_reset_if_tty(file);
    fg->black     = get_fg_color_if_tty(BLACK, file);
    fg->red       = get_fg_color_if_tty(RED, file);
    fg->green     = get_fg_color_if_tty(GREEN, file);
    fg->yellow    = get_fg_color_if_tty(YELLOW, file);
    fg->blue      = get_fg_color_if_tty(BLUE, file);
    fg->magenta   = get_fg_color_if_tty(MAGENTA, file);
    fg->cyan      = get_fg_color_if_tty(CYAN, file);
    fg->white     = get_fg_color_if_tty(WHITE, file);
    fg->gray      = get_fg_color_if_tty(GRAY, file);
    fg->l_red     = get_fg_color_if_tty(L_RED, file);
    fg->l_green   = get_fg_color_if_tty(L_GREEN, file);
    fg->l_yellow  = get_fg_color_if_tty(L_YELLOW, file);
    fg->l_blue    = get_fg_color_if_tty(L_BLUE, file);
    fg->l_magenta = get_fg_color_if_tty(L_MAGENTA, file);
    fg->l_cyan    = get_fg_color_if_tty(L_CYAN, file);
    fg->l_white   = get_fg_color_if_tty(L_WHITE, file);
}

void console_set_bg_colors_if_tty(bgColors* bg, FILE* file) {
    bg->reset     = get_reset_if_tty(file);
    bg->black     = get_bg_color_if_tty(BLACK, file);
    bg->red       = get_bg_color_if_tty(RED, file);
    bg->green     = get_bg_color_if_tty(GREEN, file);
    bg->yellow    = get_bg_color_if_tty(YELLOW, file);
    bg->blue      = get_bg_color_if_tty(BLUE, file);
    bg->magenta   = get_bg_color_if_tty(MAGENTA, file);
    bg->cyan      = get_bg_color_if_tty(CYAN, file);
    bg->white     = get_bg_color_if_tty(WHITE, file);
    bg->gray      = get_bg_color_if_tty(GRAY, file);
    bg->l_red     = get_bg_color_if_tty(L_RED, file);
    bg->l_green   = get_bg_color_if_tty(L_GREEN, file);
    bg->l_yellow  = get_bg_color_if_tty(L_YELLOW, file);
    bg->l_blue    = get_bg_color_if_tty(L_BLUE, file);
    bg->l_magenta = get_bg_color_if_tty(L_MAGENTA, file);
    bg->l_cyan    = get_bg_color_if_tty(L_CYAN, file);
    bg->l_white   = get_bg_color_if_tty(L_WHITE, file);
}

void console_set_output_info(outputInfo* out, FILE* file) {
    out->width = console_get_width(file);
    if(out->width < 1) {
        out->width = 80;
    }
    out->reset = get_reset_if_tty(file);
    console_set_fg_colors_if_tty(&(out->fg), file);
    console_set_bg_colors_if_tty(&(out->bg), file);
}