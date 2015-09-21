#include "TCPSocket.hpp"

#include <string.h>
#include <errno.h>
#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#define closesocket(s) ::close(s)
#elif _WIN32
#include "Win.hpp"
#endif

const int TCPSocket::DefaultBacklog = 5;

TCPSocket::TCPSocket() : socket(0), connected(false), listening(false),
    error(false), tls(false) { }

TCPSocket::~TCPSocket() {
    if (connected) {
        close();
    }
}

void TCPSocket::connect(const char *ip_address, unsigned short port) throw (TCPSocketException) {
    unsigned long address;
    struct sockaddr_in server;
    struct hostent *host_info;
    int rv;

    /* check states */
    check_states();

    /* create socket */
    if ((socket = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        throw TCPSocketException("Failed to create socket.");
    }

    /* convert string to Internet address */
    if ((address = inet_addr(ip_address)) != INADDR_NONE) {
        memcpy(&server.sin_addr, &address, sizeof(address));
    } else {
        host_info = gethostbyname(ip_address);
        if (!host_info) {
            closesocket(socket);
            throw TCPSocketException("Unknown server: " + std::string(ip_address));
        }
        memcpy(&server.sin_addr, host_info->h_addr, host_info->h_length);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    /* set to non blocking mode */
#ifdef __unix__
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socket, F_SETFL, flags);
#elif _WIN32
    u_long mode = 0;
    ioctlsocket(socket, FIONBIO, &mode);
#endif

    /* open connection */
    disconnecting = false;
    rv = ::connect(socket, reinterpret_cast<struct sockaddr *>(&server), sizeof(server));
    if (rv < 0) {
        if (errno == EINPROGRESS) {
            int valopt;
            struct timeval tv;
            fd_set myset;
            socklen_t lon;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            while (true) {
                FD_ZERO(&myset);
                FD_SET(socket, &myset);
                if (select(socket + 1, 0, &myset, 0, &tv) > 0) {
                   lon = sizeof(int);
#ifdef __unix__
                   getsockopt(socket, SOL_SOCKET, SO_ERROR, static_cast<void *>(&valopt), &lon);
#elif _WIN32
                   char cvalopt = 0;
                   getsockopt(socket, SOL_SOCKET, SO_ERROR, &cvalopt, &lon);
                   valopt = static_cast<int>(cvalopt);
#endif
                   if (valopt) {
                       std::string err("Cannot connect to server: ");
                       err.append(strerror(valopt));
                       throw TCPSocketException(err);
                   } else {
                       connected = true;
                       break;
                   }
                }
#ifdef __unix__
                usleep(100000);
#elif _WIN32
                Sleep(1);
#endif
                if (disconnecting) return;
            }
            if (!connected) {
                throw TCPSocketException("Cannot connect to server, timed out.");
            }
        } else {
            std::string err("Cannot connect to server: ");
            err.append(strerror(errno));
            throw TCPSocketException(err);
        }
    }
    else if (rv == 0) {
        connected = true;
    }

    /* set to blocking mode again */
#ifdef __unix__
    flags = fcntl(socket, F_GETFL, 0);
    flags &= (~O_NONBLOCK);
    fcntl(socket, F_SETFL, flags);
#elif _WIN32
    mode = 0;
    ioctlsocket(socket, FIONBIO, &mode);
#endif
}

bool TCPSocket::activity(time_t sec, long usec) throw (TCPSocketException) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    timeval timeout;
#ifdef __unix__
    timeout.tv_sec = sec;
#elif _WIN32
    timeout.tv_sec = static_cast<long>(sec);
#endif
    timeout.tv_usec = usec;

    int rv = select(socket + 1, &fds, 0, 0, &timeout);

    if (rv < 0) {
        throw TCPSocketException("Activity monitoring failed.");
    }

    if (listening && !connected) {
        return (FD_ISSET(socket, &fds) ? true : false);
    }

    if (FD_ISSET(socket, &fds)) {
        int n = 0;
#ifdef __unix__
        ioctl(socket, FIONREAD, &n);
#elif _WIN32
        u_long r = 0;
        ioctlsocket(socket, FIONREAD, &r);
        n = static_cast<int>(r);
#endif
        if (!n) {
            error = true;
            throw TCPSocketException("TCPSocket has closed.");
        }
        return true;
    }

    return false;
}

void TCPSocket::listen(const char *address, unsigned short port, int backlog) throw (TCPSocketException) {
    struct sockaddr_in server;
    int rv;

    /* check states */
    check_states();

    /* create socket */
    if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw TCPSocketException("Cannot create socket.");
    }

    /* setup listener socket */
#ifdef __unix__
    const int yes = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
#elif _WIN32
    const char yes = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
#endif
    server.sin_family = AF_INET;
    if (address) {
        server.sin_addr.s_addr = inet_addr(address);
    } else {
        server.sin_addr.s_addr = htons(INADDR_ANY);
    }
    server.sin_port = htons(port);
    rv = bind(socket, reinterpret_cast<struct sockaddr *>(&server), sizeof(server));
    if (rv < 0) {
        closesocket(socket);
        throw TCPSocketException("Bind failed: " + std::string(strerror(errno)));
    }

    /* listen */
    listening = true;
    rv = ::listen(socket, backlog);
    if (rv < 0) {
        listening = connected = false;
        closesocket(socket);
        throw TCPSocketException("Listen failed: " + std::string(strerror(errno)));
    }
}

void TCPSocket::listen(const char *address, unsigned short port) throw (TCPSocketException) {
    listen(address, port, DefaultBacklog);
}

void TCPSocket::listen(unsigned short port, int backlog) throw (TCPSocketException) {
    listen(0, port, backlog);
}

void TCPSocket::listen(unsigned short port) throw (TCPSocketException) {
    listen(0, port, DefaultBacklog);
}

void TCPSocket::accept(const TCPSocket& socket) throw (TCPSocketException) {
    struct sockaddr_in client;
    socklen_t client_len;

    /* check states */
    check_states();
    if (!socket.listening) {
        throw TCPSocketException("Invalid listener socket.");
    }

    /* accept socket */
    client_len = sizeof(client_len);
    this->socket = ::accept(socket.socket, reinterpret_cast<struct sockaddr *>(&client), &client_len);
    if (this->socket < 0) {
        throw TCPSocketException("Accept failed: " + std::string(strerror(errno)));
    }
    connected = true;
}

void TCPSocket::close() {
    disconnecting = true;
    if (connected) {
#ifdef __unix__
        ::shutdown(socket, SHUT_RDWR);
#elif _WIN32
        ::shutdown(socket, SD_BOTH);
#else
    #error What kind of os is that?
#endif
        closesocket(socket);
        connected = listening = false;
    } else if (listening) {
#ifdef __unix__
        ::shutdown(socket, SHUT_RDWR);
#elif _WIN32
        ::shutdown(socket, SD_BOTH);
#endif
        closesocket(socket);
        connected = listening = false;
    }
}

size_t TCPSocket::send(const char *buffer, size_t size) throw (TCPSocketException) {
    if (error || !connected) {
        throw TCPSocketException("Send failed, socket is closed.");
    }

    int rv = ::send(socket, buffer, size, 0);
    if (rv < 0) {
        throw TCPSocketException("Send failed: " + std::string(strerror(errno)));
    }

    return static_cast<size_t>(rv);
}

size_t TCPSocket::send(const std::string& buffer) throw (TCPSocketException) {
    return send(buffer.c_str(), buffer.length());
}

size_t TCPSocket::receive(void *buffer, size_t size) throw (TCPSocketException) {
    if (error || !connected) {
        throw TCPSocketException("Receive failed, socket is closed.");
    }

#ifdef __unix__
    int rv = recv(socket, buffer, size, 0);
#elif _WIN32
    int rv = recv(socket, static_cast<char *>(buffer), size, 0);
#endif
    if (rv < 0) {
        throw TCPSocketException("Receive failed: " + std::string(strerror(errno)));
    }

    return static_cast<size_t>(rv);
}

bool TCPSocket::get_error() const {
    return error;
}

bool TCPSocket::is_connected() const {
    return connected;
}

unsigned short TCPSocket::get_port() throw (TCPSocketException) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket, (struct sockaddr *)&sin, &len) == -1) {
        throw TCPSocketException("Can't get port: " + std::string(strerror(errno)));
    }

    return ntohs(sin.sin_port);
}

unsigned long TCPSocket::get_address() throw (TCPSocketException) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket, (struct sockaddr *)&sin, &len) == -1) {
        throw TCPSocketException("Can't get port: " + std::string(strerror(errno)));
    }

    return sin.sin_addr.s_addr;
}

void TCPSocket::check_states() throw (TCPSocketException) {
    /* already connected? */
    if (connected) {
        throw TCPSocketException("TCPSocket already in use.");
    }
}
