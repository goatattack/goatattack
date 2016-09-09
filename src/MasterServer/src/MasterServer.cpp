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

#include "MasterServer.hpp"
#include "Timing.hpp"
#include "TCPSocket.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>

Record::Record(const std::string& address, uint16_t port)
    : address(address), port(port), last_update(time(0)), delete_me(false) { }

Record::~Record() { }

MasterServer::MasterServer(uint16_t heartbeat_port, uint16_t query_port, const char *filename)
    throw (MasterServerException)
    : udp_socket(heartbeat_port), query_port(query_port),
      filename(filename), last_write(0) { }

MasterServer::~MasterServer() { }

void MasterServer::run() throw (MasterServerException) {
    char buffer[1024];
    uint32_t host;
    uint16_t port;
    size_t sz = 0;
    TCPSocket listener;

    listener.listen(query_port);
    while (true) {
        do {
            sz = udp_socket.recv(buffer, sizeof buffer, &host, &port);
            if (sz) {
                add_entry(host, buffer, sz);
            }
        } while (sz);

        delete_old_entries();

        if (listener.activity(0, 100000)) {
            TCPSocket client;
            client.accept(listener);
            for (Records::iterator it = records.begin(); it != records.end(); it++) {
                Record& r = *it;
                sprintf(buffer, "%s %hu\n", r.address.c_str(), r.port);
                client.send(buffer, strlen(buffer));
            }
        }

        dump_entries();
    }
}

void MasterServer::add_entry(uint32_t host, char *buffer, size_t sz) {
    static char cvbuf[512];

    if (sz < 10) {
        uint32_t my_host = htonl(host);
        inet_ntop(AF_INET, &my_host, cvbuf, sizeof cvbuf);
        std::string address(cvbuf);

        bool found = false;
        buffer[sz] = 0;
        int port = atoi(buffer);
        if (port) {
            for (Records::iterator it = records.begin(); it != records.end(); it++) {
                Record& r = *it;
                if (r.address == address && r.port == port) {
                    found = true;
                    r.last_update = time(0);
                    break;
                }
            }
        }

        if (!found) {
            records.push_back(Record(address, port));
        }
    }
}

template <class T> static bool erase_entry(T& entry) {
    return (entry.delete_me);
}

void MasterServer::delete_old_entries() {
    time_t now = time(0);
    for (Records::iterator it = records.begin(); it != records.end(); it++) {
        Record& r = *it;
        if (now - r.last_update > 30) {
            r.delete_me = true;
        }
    }

    records.erase(std::remove_if(records.begin(), records.end(),
        erase_entry<Record>), records.end());
}

void MasterServer::dump_entries() {
    if (filename.length()) {
        time_t now = time(0);
        if (now - last_write > 30) {
            char buffer[1024];
            FILE *f = fopen(filename.c_str(), "w");
            if (f) {
                for (Records::iterator it = records.begin(); it != records.end(); it++) {
                    Record& r = *it;
                    sprintf(buffer, "%s %hu\n", r.address.c_str(), r.port);
                    fwrite(buffer, 1, strlen(buffer), f);
                }
                fclose(f);
            }
            last_write = time(0);
        }
    }
}