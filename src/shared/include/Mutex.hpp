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

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include "Exception.hpp"

#ifdef __unix__
#include <pthread.h>
#elif _WIN32
#include "Win.hpp"
#endif

class MutexException : public Exception {
public:
    MutexException(const char *msg) : Exception(msg) { }
    MutexException(std::string msg) : Exception(msg) { }
};

class Mutex {
private:
    Mutex(const Mutex& rhs);
    Mutex& operator=(const Mutex& rhs);

    typedef struct {
#ifdef __unix__
        pthread_mutex_t h_mutex;
#else
        HANDLE h_mutex;
#endif
    } mutex_t;

public:
    Mutex();
    virtual ~Mutex();

    void lock();
    void unlock();

    void enter_scope();
    void leave_scope();

private:
    mutex_t *mutex;
};

#endif
