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

#ifndef _MASTERSERVER_HPP_
#define _MASTERSERVER_HPP_

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
    MasterServer(uint16_t heartbeat_port, uint16_t query_port, const char *filename, int refresh);
    virtual ~MasterServer();

    void run();

private:
    UDPSocket udp_socket;
    uint16_t query_port;
    std::string filename;
    int refresh;
    time_t last_write;

    Records records;

    void add_entry(uint32_t host, char *buffer, size_t sz);
    void delete_old_entries();
    void dump_entries();
};

#endif
