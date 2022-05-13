#ifndef KSYS_INFO_H
#define KSYS_INFO_H

#include <stdint.h>
#include <stddef.h>

#if defined(__GNUC__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

#define ARCH_UNKNOWN  0x0000U
#define X86_32        0x0100U
#define X86_64        0x0101U
#define ARM_64        0x0200U

#if defined(__x86_64__) || defined(_M_X64)
    #define CURRENT_ARCH X86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define CURRENT_ARCH X86_32
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define CURRENT_ARCH ARM_64
#else
    #define CURRENT_ARCH ARCH_UNKNOWN
#endif

#define OS_UNKNOWN   0x0000U
#define OS_LINUX     0x0100U
#define OS_WINDOWS   0x0200U
#define OS_UNIX_LIKE 0x0F00U

#if defined(_WIN32) || defined(__WIN32)
    #define CURRENT_OS OS_WINDOWS
#elif defined(__linux__)
    #define CURRENT_OS OS_LINUX
#elif defined(__unix__)
    #define CURRENT_OS OS_UNIX_LIKE
#else
    #define CURRENT_OS OS_UNKNOWN
#endif

unsigned get_arch();
unsigned get_os();
size_t   get_memory_page_size();

#endif