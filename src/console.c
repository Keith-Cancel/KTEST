#include "console.h"
#include "sys-info.c"

#include <locale.h>

#if CURRENT_OS == OS_WINDOWS
#include <windows.h>
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