#ifndef K_TIMER_H
#define K_TIMER_H

#include "sys-info.h"
#include <stdint.h>
#include <math.h>

static void timer_format_ns(double amt, char buffer[14]) {
    char units[5][3] = {
        { "ns" },
        { "us" },
        { "ms" },
        { "s"  },
        { "ks" }
    };
    int i = 0;
    while(fabs(amt) > 1000 && i < 5) {
        i++;
        amt /= 1000;
    }
    snprintf(buffer, 14, i == 0 ? "%.0f%s" : "%.2f%s", amt, units[i]);
}

#if CURRENT_OS == OS_LINUX || CURRENT_OS == OS_UNIX_LIKE
#include <time.h>

typedef struct {
    struct timespec t0;
    struct timespec t1;
} timerData;

static FORCE_INLINE void timer_start(timerData* data) {
    clock_gettime(CLOCK_MONOTONIC, &(data->t0));
}

static FORCE_INLINE void timer_stop(timerData* data) {
    clock_gettime(CLOCK_MONOTONIC, &(data->t1));
}

static void timer_get_str(const timerData* data, char buffer[14]) {
    uint32_t n0     = data->t0.tv_nsec;
    uint32_t n1     = data->t1.tv_nsec;
    uint32_t borrow = n0 > n1;
    uint32_t nsec   = n1 - n0;
    int32_t  secs   = data->t1.tv_sec - data->t0.tv_sec - borrow;
    if(secs != 0) {
        // Just handle seconds in it's own case to make things simpler
        // to avoid having to juggle 2 different variables for the next
        // part
        int tens = rintf(((float)nsec / 1000000000.0f) * 10.0f);
        snprintf(buffer, 14, "%d.%ds", secs, tens);
        return;
    }
    timer_format_ns(nsec, buffer);
}

#elif CURRENT_OS == OS_WINDOWS
#include <windows.h>
typedef struct {
    LARGE_INTEGER t0;
    LARGE_INTEGER t1;
} timerData;

static FORCE_INLINE void timer_start(timerData* data) {
    QueryPerformanceCounter(&(data->t0));
}

static FORCE_INLINE void timer_stop(timerData* data) {
    QueryPerformanceCounter(&(data->t1));
}

static void timer_get_str(const timerData* data, char buffer[14]) {
    LARGE_INTEGER freq = { 0 };
    if(!QueryPerformanceFrequency(&freq) || freq.QuadPart == 0) {
        snprintf(buffer, 14, "Failed!");
    }
    int64_t time  = data->t1.QuadPart - data->t0.QuadPart;
    double  scale = (double)1000000000.0f / ((double)freq.QuadPart);
    double  amt   = time * scale; // Time in nano seconds
    timer_format_ns(amt, buffer);
}

#endif

#endif