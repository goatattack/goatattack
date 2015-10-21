#ifndef MUTEX_HPP
#define MUTEX_HPP

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
    Mutex() throw (MutexException);
    virtual ~Mutex();

    void lock();
    void unlock();

    void enter_scope();
    void leave_scope();

private:
    mutex_t *mutex;
};

#endif
