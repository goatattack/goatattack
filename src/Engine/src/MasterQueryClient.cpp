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

#include "MasterQueryClient.hpp"
#include "Timing.hpp"
#include "MasterQuery.hpp"

const int MaxTries = 10;
MasterQueryClient::MasterQueryClient(I18N& i18n, MasterQuery& master_query, hostaddr_t host, hostport_t port) throw (Exception)
    : GameserverInformation(host, port), MessageSequencer(i18n, host, port), master_query(master_query),
      received(false), tries(0)
{
    get_now(last_update);
}

MasterQueryClient::~MasterQueryClient() { }

void MasterQueryClient::refresh() {
    received = false;
    request_server_info(host, port);
}

void MasterQueryClient::timed_refresh(gametime_t& now) {
    if (!received && tries < MaxTries) {
        if (diff_ms(last_update, now) > 250) {
            refresh();
            last_update = now;
            tries++;
        }
    }
}

void MasterQueryClient::event_status(hostaddr_t host, hostport_t port, const std::string& name,
    int max_clients, int cur_clients, ms_t ping_time, bool secured,
    int protocol_version) throw (Exception)
{
    this->server_name = name;
    this->map_name = "";
    this->max_clients = max_clients;
    this->cur_clients = cur_clients;
    this->ping_time = ping_time;
    this->secured = secured;
    this->protocol_version = protocol_version;

    received = true;

    master_query.locked_sort();
}
