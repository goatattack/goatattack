/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TIMING_HPP_
#define _TIMING_HPP_

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
