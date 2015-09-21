#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include "Exception.hpp"

#ifdef _MSC_VER
    #pragma warning( disable : 4290 )
#endif

class TCPSocketException : public Exception {
public:
    TCPSocketException(const char *msg) : Exception(msg) { }
    TCPSocketException(std::string msg) : Exception(msg) { }
};

class TCPSocket {
private:
    TCPSocket(const TCPSocket&);
    TCPSocket& operator=(const TCPSocket&);

public:
    TCPSocket();
    virtual ~TCPSocket();

    void connect(const char *ip_address, unsigned short port) throw (TCPSocketException);
    bool activity(time_t sec, long usec) throw (TCPSocketException);
    void listen(const char *address, unsigned short port, int backlog) throw (TCPSocketException);
    void listen(const char *address, unsigned short port) throw (TCPSocketException);
    void listen(unsigned short port, int backlog) throw (TCPSocketException);
    void listen(unsigned short port) throw (TCPSocketException);
    void accept(const TCPSocket& socket) throw (TCPSocketException);
    void close();

    size_t send(const char *buffer, size_t size) throw (TCPSocketException);
    size_t send(const std::string& buffer) throw (TCPSocketException);
    size_t receive(void *buffer, size_t size) throw (TCPSocketException);
    bool get_error() const;
    bool is_connected() const;
    unsigned short get_port() throw (TCPSocketException);
    unsigned long get_address() throw (TCPSocketException);

private:
    static const int DefaultBacklog;

    int socket;
    bool connected;
    bool listening;
    bool error;
    bool tls;
    bool disconnecting;

    void check_states() throw (TCPSocketException);
};

#endif // TCPSOCKET_HPP
