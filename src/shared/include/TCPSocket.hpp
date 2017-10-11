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

#ifndef _TCPSOCKET_HPP_
#define _TCPSOCKET_HPP_

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

    void connect(const char *ip_address, unsigned short port);
    bool activity(time_t sec, long usec);
    void listen(const char *address, unsigned short port, int backlog);
    void listen(const char *address, unsigned short port);
    void listen(unsigned short port, int backlog);
    void listen(unsigned short port);
    void accept(const TCPSocket& socket);
    void close();

    size_t send(const char *buffer, size_t size);
    size_t send(const std::string& buffer);
    size_t receive(void *buffer, size_t size);
    bool get_error() const;
    bool is_connected() const;
    unsigned short get_port();
    unsigned long get_address();

private:
    static const int DefaultBacklog;

    int socket;
    bool connected;
    bool listening;
    bool error;
    bool tls;
    bool disconnecting;

    void check_states();
};

#endif
