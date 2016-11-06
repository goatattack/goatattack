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

#ifndef _GAMESERVERINFORMATION_HPP_
#define _GAMESERVERINFORMATION_HPP_

#include "Protocol.hpp"

#include <string>
#include <vector>

enum ServerSort {
    Ascending = 0,
    Descending
};

class GameserverInformation {
private:
    GameserverInformation(const GameserverInformation&);
    GameserverInformation& operator=(const GameserverInformation&);

public:
    GameserverInformation();
    GameserverInformation(hostaddr_t host, hostport_t port);
    virtual ~GameserverInformation();

    hostaddr_t host;
    hostport_t port;
    std::string server_name;
    std::string map_name;
    pico_size_t max_clients;
    pico_size_t cur_clients;
    int protocol_version;
    bool secured;
    ms_t ping_time;
    time_t last_status;
};

typedef std::vector<GameserverInformation *> Hosts;

class ServerList {
private:
    ServerList(const ServerList&);
    ServerList& operator=(const ServerList&);

public:
    ServerList();
    virtual ~ServerList();

    const Hosts& get_hosts() const;
    ServerSort sort(int index, ServerSort dir);
    ServerSort sort(int index);
    ServerSort sort();

private:
    typedef bool (*ServerSorter)(GameserverInformation *lhs, GameserverInformation *rhs);

    int sort_index;
    ServerSort sort_dir;
    ServerSorter server_sorter;

protected:
    Hosts hosts;

private:
    ServerSorter get_server_sorter();
};

#endif
