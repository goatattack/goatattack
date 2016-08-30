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

#include "Mutex.hpp"

#include <algorithm>

Mutex::Mutex() throw (MutexException) {
    try {
        mutex = new mutex_t;
    } catch (const std::exception& e) {
        throw MutexException(e.what());
    }
#ifdef __unix__
    pthread_mutex_init(&mutex->h_mutex, NULL);
#elif _WIN32
    mutex->h_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
}

Mutex::~Mutex() {
#ifdef __unix__
    pthread_mutex_destroy(&mutex->h_mutex);
#elif _WIN32
    CloseHandle(mutex->h_mutex);
#endif
    delete mutex;
}

void Mutex::lock() {
#ifdef __unix__
    pthread_mutex_lock(&mutex->h_mutex);
#elif _WIN32
    WaitForSingleObject(mutex->h_mutex, INFINITE);
#endif
}

void Mutex::unlock() {
#ifdef __unix__
    pthread_mutex_unlock(&mutex->h_mutex);
#elif _WIN32
    ReleaseMutex(mutex->h_mutex);
#endif
}

void Mutex::enter_scope() {
    lock();
}

void Mutex::leave_scope() {
    unlock();
}