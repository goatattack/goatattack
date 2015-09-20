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

ScopeMutex::ScopeMutex(Mutex& mtx) : mtx(mtx) {
    mtx.lock();
}

ScopeMutex::~ScopeMutex() {
    mtx.unlock();
}
