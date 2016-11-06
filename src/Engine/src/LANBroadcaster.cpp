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

#include "LANBroadcaster.hpp"
#include "Scope.hpp"

LANBroadcaster::LANBroadcaster(I18N& i18n, hostport_t port) throw (LANBroadcasterException, UDPSocketException)
    : MessageSequencer(i18n, port, 0, "", ""), i18n(i18n), port(port), running(false)
{
    refresh();
    start();
}

LANBroadcaster::~LANBroadcaster() {
    stop();
    cleanup();
}

void LANBroadcaster::start() throw (LANBroadcasterException) {
    if (!running) {
        running = true;
        if (!thread_start()) {
            throw LANBroadcasterException(i18n(I18N_THREAD_FAILED));
        }
    }
}

void LANBroadcaster::stop() {
    if (running) {
        running = false;
        thread_join();
    }
}

void LANBroadcaster::refresh() throw (Exception) {
    request_server_info(INADDR_BROADCAST, port);
}

void LANBroadcaster::clear() {
    cleanup();
}

Mutex& LANBroadcaster::get_mutex() {
    return mtx;
}

void LANBroadcaster::event_status(hostaddr_t host, hostport_t port,
    const std::string& name, int max_clients, int cur_clients, ms_t ping_time,
    bool secured, int protocol_version) throw (Exception)
{
    GameserverInformation *info = 0;
    bool found = false;

    /* find in list */
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        info = *it;
        if (info->host == host && info->port == port) {
            found = true;
            break;
        }
    }

    /* add/update information */
    if (!found) {
        info = new GameserverInformation;
        hosts.push_back(info);
    }
    info->server_name = name;
    info->host = host;
    info->port = port;
    info->max_clients = max_clients;
    info->cur_clients = cur_clients;
    info->ping_time = ping_time;
    info->secured = secured;
    info->protocol_version = protocol_version;
    info->last_status = time(0);

    sort();
}

void LANBroadcaster::cleanup() {
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        delete *it;
    }
    hosts.clear();
}

void LANBroadcaster::thread() {
    int counter = 0;
    while (running) {
        {
            Scope<Mutex> lock(mtx);
            while(cycle());
        }

        /* remove old servers */
        counter++;
        if (counter > 1000) {
            counter = 0;
            bool again;
            time_t now = time(0);
            do {
                again = false;
                for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
                    GameserverInformation *info = *it;
                    if (now - info->last_status >= 5) {
                        hosts.erase(it);
                        again = true;
                        break;
                    }
                }
            } while (again);
        }
        wait_ns(1000000);
    }
}
