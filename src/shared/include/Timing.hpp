#ifndef TIMING_HPP
#define TIMING_HPP

#include <time.h>
#include <stdint.h>

#ifdef _WIN32
#include "Win.hpp"
#endif

typedef struct timespec gametime_t;
typedef long ms_t;
typedef long ns_t;

void init_hpet();
void get_now(gametime_t& time);
ms_t diff_ms(const gametime_t& starttime, const gametime_t& endtime);
ns_t diff_ns(const gametime_t& starttime, const gametime_t& endtime);
void add_ns(ns_t ns, gametime_t& to);
void sub_ns(ns_t ns, gametime_t& from);
void wait_ns(ns_t ns);
void wait_ms(ms_t ms);

#endif