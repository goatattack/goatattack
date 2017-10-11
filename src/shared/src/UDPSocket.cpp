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

#include "UDPSocket.hpp"

#include <cerrno>
#include <cstring>
#ifdef __unix__
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

UDPSocket::UDPSocket() : port(0) {
    create_socket(0);
}

UDPSocket::UDPSocket(unsigned short port) : port(port) {
    create_socket(port);
}

UDPSocket::~UDPSocket() {
    closesocket(socket);
}

ssize_t UDPSocket::send(uint32_t host, uint16_t port, void *buffer, size_t length) {
    addr.sin_addr.s_addr = htonl(host);
    addr.sin_port = htons(port);

    ssize_t sz = sendto(socket, static_cast<const char *>(buffer), length, 0, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
#ifdef __unix__
    if (sz < 0) {
        throw UDPSocketException("Error sending packet: " + std::string(strerror(errno)));
    }
#elif _WIN32
    if (sz < 0) {
        sz = 0;
    }
#endif

    return sz;
}

ssize_t UDPSocket::recv(char *buffer, size_t length, uint32_t *host, uint16_t *port) {
    struct sockaddr_in ao;
    socklen_t ao_len = sizeof(ao);

    ssize_t sz = recvfrom(socket, buffer, length, 0, reinterpret_cast<struct sockaddr *>(&ao), &ao_len);
#ifdef __unix__
    if (sz < 0) {
        if (errno != EAGAIN) {
            throw UDPSocketException("Error receiving packet: " + std::string(strerror(errno)));
        }
        return 0;
    }
#elif _WIN32
    if (sz < 0) {
        return 0;
    }
#endif

    if (host) *host = ntohl(ao.sin_addr.s_addr);
    if (port) *port = ntohs(ao.sin_port);

    return sz;
}

void UDPSocket::set_port(unsigned short port) {
    if (!this->port && port != this->port) {
        throw UDPSocketException("Cannot change port of a client socket");
    }
    if (port != this->port) {
        closesocket(socket);
        create_socket(port);
    }
}

unsigned short UDPSocket::get_port() const {
    return port;
}

void UDPSocket::create_socket(unsigned short port) {
    socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket < 0) {
        throw UDPSocketException("Creating socket failed: " + std::string(strerror(errno)));
    }

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
#else
    fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
#endif

#ifdef __unix__
    int bc = 1;
    int rv = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &bc, sizeof bc);
#else
    const char bc = 1;
    int rv = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &bc, sizeof bc);
#endif
    if (rv < 0) {
        closesocket(socket);
        throw UDPSocketException("Could not set to broadcast: " + std::string(strerror(errno)));
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->port = port;

    if (port) {
        if (bind(socket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1) {
            closesocket(socket);
            throw UDPSocketException("UDPSocket binding failed: " + std::string(strerror(errno)));
        }
    }
}
