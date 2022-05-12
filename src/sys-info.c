#include "sys-info.h"

unsigned get_arch() {
    return CURRENT_ARCH;
}

unsigned get_os() {
    return CURRENT_OS;
}

#if CURRENT_OS == OS_WINDOWS
#include <windows.h>

size_t get_memory_page_size() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

#elif CURRENT_OS == OS_UNIX_LIKE || CURRENT_OS == OS_LINUX
#include <unistd.h>

size_t get_memory_page_size() {
    return sysconf(_SC_PAGESIZE);
}

#else
    #error "Not ported to this OS"
#endif