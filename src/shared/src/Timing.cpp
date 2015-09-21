#include "Timing.hpp"

#ifdef _WIN32
#include "Win.hpp"
static __int64 __freq;
static __int64 __qry;
double __ticks;
static double __timer_millis_mult;
#elif __APPLE__
#include <mach/mach_time.h>
mach_timebase_info_data_t __mach_timer_info;
static double __timer_millis_mult;
#endif

void init_hpet() {
#ifdef __APPLE__
    mach_timebase_info(&__mach_timer_info);
    __timer_millis_mult = (static_cast<double>(__mach_timer_info.numer) / (static_cast<double>(__mach_timer_info.denom))) / 1000000000;
#elif _WIN32
    QueryPerformanceFrequency((LARGE_INTEGER*)&__freq);
    __timer_millis_mult = static_cast<double>(__freq);
#endif
}

#include <iostream>
#include <cstdio>

void get_now(gametime_t& time) {
#ifdef __unix__
#ifdef __APPLE__
    double d = static_cast<double>(mach_absolute_time()) * __timer_millis_mult;
    time.tv_sec = static_cast<time_t>(d);
    time.tv_nsec = static_cast<long>((d - time.tv_sec) * 1000000000);
#else
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
#endif
#elif _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&__qry);
    double d = static_cast<double>(__qry) / __timer_millis_mult;
    time.tv_sec = static_cast<int>(d);
    time.tv_nsec = static_cast<long>((d - time.tv_sec) * 1000000000);
#endif
}

ms_t diff_ms(const gametime_t& starttime, const gametime_t& endtime) {
    ms_t ms = static_cast<ms_t>((endtime.tv_sec - starttime.tv_sec)) * 1000;
    ms += (endtime.tv_nsec - starttime.tv_nsec) / 1000000;

    return ms;
}

ns_t diff_ns(const gametime_t& starttime, const gametime_t& endtime) {
    ns_t ns = static_cast<ns_t>(endtime.tv_sec - starttime.tv_sec);
    if (ns < 2) {
        ns *= 1000000000;
        ns += (endtime.tv_nsec - starttime.tv_nsec);
    } else {
        /* 32bit overflow */
        ns = 0;
    }

    return ns;
}

void add_ns(ns_t ns, gametime_t& to) {
    to.tv_nsec += ns;
    if (to.tv_nsec > 999999999) {
        to.tv_sec++;
        to.tv_nsec -= 1000000000;
    }
}

void sub_ns(ns_t ns, gametime_t& from) {
    from.tv_nsec -= ns;
    if (from.tv_nsec < 0) {
        from.tv_sec--;
        from.tv_nsec += 1000000000;
    }
}

void wait_ns(ns_t ns) {
#ifdef __unix__
    gametime_t wait;
    gametime_t rem;

    wait.tv_sec = 0;
    wait.tv_nsec = ns;
    nanosleep(&wait, &rem);
#elif _WIN32
    Sleep(ns / 1000000);
#endif
}

void wait_ms(ms_t ms) {
#ifdef __unix__
    gametime_t wait;
    gametime_t rem;

    wait.tv_sec = 0;
    wait.tv_nsec = ms * 1000000;
    nanosleep(&wait, &rem);
#elif _WIN32
    Sleep(ms);
#endif
}
