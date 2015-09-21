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
