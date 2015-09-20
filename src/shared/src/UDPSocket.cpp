#include "UDPSocket.hpp"

#include <cerrno>
#include <cstring>
#ifdef __unix__
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

UDPSocket::UDPSocket() throw (UDPSocketException) : port(0) {
    create_socket(0);
}

UDPSocket::UDPSocket(unsigned short port) throw (UDPSocketException) : port(port) {
    create_socket(port);
}

UDPSocket::~UDPSocket() {
    closesocket(socket);
}

ssize_t UDPSocket::send(uint32_t host, uint16_t port, void *buffer, size_t length) throw (UDPSocketException) {
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

ssize_t UDPSocket::recv(char *buffer, size_t length, uint32_t *host, uint16_t *port) throw (UDPSocketException) {
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

void UDPSocket::create_socket(unsigned short port) throw (UDPSocketException) {
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

    if (port) {
        if (bind(socket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1) {
			closesocket(socket);
            throw UDPSocketException("UDPSocket binding failed: " + std::string(strerror(errno)));
        }
    }
}
