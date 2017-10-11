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

#include "MasterQuery.hpp"
#include "TCPSocket.hpp"
#include "Scope.hpp"

#include <cstdlib>

MasterQuery::MasterQuery(I18N& i18n, const std::string& masterserver, hostport_t masterport)
    : i18n(i18n), masterserver(masterserver), masterport(masterport), running(false)
{
    refresh();
    start();
}

MasterQuery::~MasterQuery() {
    stop();
    cleanup();
}

void MasterQuery::start() {
    if (!running) {
        running = true;
        if (!thread_start()) {
            throw MasterQueryException(i18n(I18N_THREAD_FAILED));
        }
    }
}

void MasterQuery::stop() {
    if (running) {
        running = false;
        thread_join();
    }
}

void MasterQuery::cycle(gametime_t *now) {
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        MasterQueryClient *client = static_cast<MasterQueryClient *>(*it);
        if (now) {
            client->timed_refresh(*now);
        }
        while (client->cycle());
    }
}

void MasterQuery::refresh() {
    TCPSocket query;
    char buffer[1024];
    std::string pending;

    try {
        query.connect(masterserver.c_str(), masterport);
        while (query.is_connected()) {
            if (query.activity(0, 100000)) {
                memset(buffer, 0, sizeof(buffer));
                size_t sz = query.receive(buffer, sizeof(buffer));
                if (sz) {
                    pending += buffer;
                }
            }
        }
    } catch(const Exception&) {
        /* chomp */
    }

    /* setup retrievers */
    cleanup();
    while (pending.length()) {
        std::string entry;
        size_t pos = pending.find('\n');
        if (pos == std::string::npos) {
            entry = pending;
            pending.clear();
        } else {
            entry = pending.substr(0, pos);
            pending = pending.substr(pos + 1);
        }

        pos = entry.find(' ');
        if (pos != std::string::npos) {
            hostaddr_t host = ntohl(inet_addr(entry.substr(0, pos).c_str()));
            hostport_t port = atoi(entry.substr(pos + 1).c_str());
            MasterQueryClient *mqc = new MasterQueryClient(i18n, *this, host, port);
            hosts.push_back(mqc);
        }
    }
}

void MasterQuery::locked_sort() {
    sort();
}

Mutex& MasterQuery::get_mutex(){
    return mtx;
}

void MasterQuery::cleanup() {
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        delete *it;
    }
    hosts.clear();
}

void MasterQuery::thread() {
    gametime_t now;
    while (running) {
        {
            get_now(now);
            Scope<Mutex> lock(mtx);
            cycle(&now);
        }
        wait_ns(1000000);
    }
}
