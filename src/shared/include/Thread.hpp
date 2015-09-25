#ifndef THREAD_HPP
#define THREAD_HPP

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
    virtual ~Thread();

protected:
    bool thread_start();
    void thread_join();
    void thread_signal(int sig);
    void thread_cancel();
    void thread_detach();
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
