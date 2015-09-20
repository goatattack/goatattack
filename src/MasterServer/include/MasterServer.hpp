#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include "Exception.hpp"
#include "UDPSocket.hpp"

#include <ctime>
#include <vector>
#include <string>

class MasterServerException : public Exception {
public:
    MasterServerException(const char *msg) : Exception(msg) { }
    MasterServerException(std::string msg) : Exception(msg) { }
};

class Record {
public:
    Record(const std::string& address, uint16_t port);
    virtual ~Record();

    std::string address;
    uint16_t port;
    time_t last_update;
    bool delete_me;
};

typedef std::vector<Record> Records;

class MasterServer {
public:
    MasterServer(uint16_t heartbeat_port, uint16_t query_port) throw (MasterServerException);
    virtual ~MasterServer();

    void run() throw (MasterServerException);

private:
    UDPSocket udp_socket;
    uint16_t query_port;

    Records records;

    void add_entry(uint32_t host, char *buffer, size_t sz);
    void delete_old_entries();
};

#endif // MASTERSERVER_HPP
