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

#ifndef _UDPSOCKET_HPP_
#define _UDPSOCKET_HPP_

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
    UDPSocket();
    UDPSocket(unsigned short port);
    ~UDPSocket();

    ssize_t send(uint32_t host, uint16_t port, void *buffer, size_t length);
    ssize_t recv(char *buffer, size_t length, uint32_t *host, uint16_t *port);
    void set_port(unsigned short port);
    unsigned short get_port() const;

protected:
#ifdef __unix__
    int socket;
#elif _WIN32
    SOCKET socket;
#endif
    unsigned short port;
    struct sockaddr_in addr;

    void create_socket(unsigned short port);
};

#endif
