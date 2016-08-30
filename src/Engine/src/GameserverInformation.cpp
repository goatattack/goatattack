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

#include "GameserverInformation.hpp"

#include <algorithm>

/* server information class */
GameserverInformation::GameserverInformation()
    : host(0), port(0), max_clients(0), cur_clients(0),
      protocol_version(0), secured(false), ping_time(0) { }

GameserverInformation::GameserverInformation(hostaddr_t host, hostport_t port)
    : host(host), port(port), max_clients(0), cur_clients(0),
      protocol_version(0), secured(false), ping_time(0) { }

GameserverInformation::~GameserverInformation() { }

/* compare functions */
static bool server_info_cmp_name_asc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->server_name < rhs->server_name;
}

static bool server_info_cmp_name_desc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->server_name > rhs->server_name;
}

static bool server_info_cmp_locked_asc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->secured < rhs->secured;
}

static bool server_info_cmp_locked_desc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->secured > rhs->secured;
}

static bool server_info_cmp_players_asc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return (lhs->max_clients - lhs->cur_clients) < (rhs->max_clients - rhs->cur_clients);
}

static bool server_info_cmp_players_desc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return (lhs->max_clients - lhs->cur_clients) > (rhs->max_clients - rhs->cur_clients);
}

static bool server_info_cmp_ping_asc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->ping_time < rhs->ping_time;
}

static bool server_info_cmp_ping_desc(GameserverInformation *lhs, GameserverInformation *rhs) {
    return lhs->ping_time > rhs->ping_time;
}

/* list with sort caps */
ServerList::ServerList() : sort_index(0), sort_dir(Ascending),
    server_sorter(get_server_sorter()) { }

ServerList::~ServerList() { }

const Hosts& ServerList::get_hosts() const {
    return hosts;
}

void ServerList::sort(int index, ServerSort dir) {
    sort_index = index;
    sort_dir = dir;
    server_sorter = get_server_sorter();
    std::sort(hosts.begin(), hosts.end(), server_sorter);
}

void ServerList::sort(int index) {
    if (index != sort_index) {
        sort(index, Ascending);
    } else {
        ServerSort new_dir = (sort_dir == Ascending ? Descending : Ascending);
        sort(index, new_dir);
    }
}

void ServerList::sort() {
    sort(sort_index, sort_dir);
}

ServerList::ServerSorter ServerList::get_server_sorter() {
    static ServerSorter server_sorters[] = {
        server_info_cmp_name_asc,
        server_info_cmp_name_desc,
        server_info_cmp_locked_asc,
        server_info_cmp_locked_desc,
        server_info_cmp_players_asc,
        server_info_cmp_players_desc,
        server_info_cmp_ping_asc,
        server_info_cmp_ping_desc
    };

    return server_sorters[sort_index * 2 + static_cast<int>(sort_dir)];
}