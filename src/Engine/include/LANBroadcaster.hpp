#ifndef LANBROADCASTER_HPP
#define LANBROADCASTER_HPP

#include "Exception.hpp"
#include "MessageSequencer.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "GameserverInformation.hpp"

class LANBroadcasterException : public Exception {
public:
    LANBroadcasterException(const char *msg) : Exception(msg) { }
    LANBroadcasterException(std::string msg) : Exception(msg) { }
};

class LANBroadcaster : public ServerList, public MessageSequencer, public Thread {
private:
    LANBroadcaster(const LANBroadcaster&);
    LANBroadcaster& operator=(const LANBroadcaster&);

public:
    LANBroadcaster(hostport_t port) throw (LANBroadcasterException, UDPSocketException);
    virtual ~LANBroadcaster();

    void start() throw (LANBroadcasterException);
    void stop();
    void refresh() throw (Exception);
    void clear();
    Mutex& get_mutex();

private:

    hostport_t port;
    bool running;

    Mutex mtx;

    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version) throw (Exception);

    void cleanup();

    virtual void thread();
};

#endif
