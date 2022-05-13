#ifndef K_TIMER_H
#define K_TIMER_H

#include "sys-info.h"
#include <stdint.h>
#include <math.h>

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
    char units[5][3] = {
        { "ns" },
        { "us" },
        { "ms" },
        { "s"  },
        { "ks" }
    };
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
    double amt = nsec;
    int   i   = 0;
    while(amt > 1000 && i < 5) {
        i++;
        amt /= 1000;
    }
    snprintf(buffer, 14, "%.1f%s", amt, units[i]);
}

#elif CURRENT_OS == OS_WINDOWS
    #error "Not ported to windows yet!"
#endif



#endif