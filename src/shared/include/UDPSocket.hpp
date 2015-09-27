#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP

#include "Exception.hpp"

#ifdef __unix__
#include <arpa/inet.h>
#define closesocket(s) ::close(s)
#elif _WIN32
#include "Win.hpp"
#endif

class UDPSocketException : public Exception {
public:
    UDPSocketException(const char *msg) : Exception(msg) { }
    UDPSocketException(std::string msg) : Exception(msg) { }
};

class UDPSocket {
private:
    UDPSocket(const UDPSocket&);
    UDPSocket& operator=(const UDPSocket&);

public:
    UDPSocket() throw (UDPSocketException);
    UDPSocket(unsigned short port) throw (UDPSocketException);
    virtual ~UDPSocket();

    ssize_t send(uint32_t host, uint16_t port, void *buffer, size_t length) throw (UDPSocketException);
    ssize_t recv(char *buffer, size_t length, uint32_t *host, uint16_t *port) throw (UDPSocketException);
    void set_port(unsigned short port) throw (UDPSocketException);
    unsigned short get_port() const;

protected:
#ifdef __unix__
    int socket;
#elif _WIN32
    SOCKET socket;
#endif
    unsigned short port;
    struct sockaddr_in addr;

    void create_socket(unsigned short port) throw (UDPSocketException);
};

#endif