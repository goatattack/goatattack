#ifndef MASTERQUERY_HPP
#define MASTERQUERY_HPP

#include "Exception.hpp"
#include "MasterQueryClient.hpp"
#include "Timing.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"

#include <vector>

class MasterQueryException : public Exception {
public:
    MasterQueryException(const char *msg) : Exception(msg) { }
    MasterQueryException(std::string msg) : Exception(msg) { }
};

class MasterQuery : public ServerList, public Thread {
private:
    MasterQuery(const MasterQuery&);
    MasterQuery& operator=(const MasterQuery&);

public:
    MasterQuery(const std::string& masterserver, hostport_t masterport)
        throw (MasterQueryException);
    virtual ~MasterQuery();

    void start() throw (MasterQueryException);
    void stop();
    void cycle(gametime_t *now);
    void refresh() throw (MasterQueryException);
    void locked_sort();

    Mutex& get_mutex();

private:
    std::string masterserver;
    hostport_t masterport;
    bool running;

    Mutex mtx;

    void cleanup();

    virtual void thread();
};

#endif
