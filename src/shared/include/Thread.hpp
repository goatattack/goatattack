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

#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#ifdef __unix__
#include <pthread.h>
#include <signal.h>
#elif _WIN32
#include "Win.hpp"
#endif

class Thread {
private:
    Thread(const Thread&);
    Thread& operator=(const Thread&);

public:
    Thread();
    ~Thread();

protected:
    bool thread_start();
    void thread_join();
    virtual void thread() = 0;

private:
#ifdef __unix__
    pthread_t t;
#else
    HANDLE t;
#endif

    static void *thread_helper(void *o);
};

#endif
