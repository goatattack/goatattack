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

#include "Thread.hpp"

Thread::Thread() : t(0) { }

Thread::~Thread() { 
#ifdef _WIN32
    CloseHandle(t);
#endif
}

bool Thread::thread_start() {
#ifdef __unix__
    return (pthread_create(&t, 0, thread_helper, this) == 0);
#elif _WIN32
    t = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(thread_helper), this, 0, NULL);
    return (t != 0);
#endif
}

void Thread::thread_join() {
#ifdef __unix__
    pthread_join(t, 0);
#elif _WIN32
    WaitForSingleObject(t, INFINITE);
#endif
}

void *Thread::thread_helper(void *o) {
    (reinterpret_cast<Thread *>(o))->thread();
    return 0;
}
