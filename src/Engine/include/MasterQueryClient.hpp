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

#ifndef _MASTERQUERYCLIENT_HPP_
#define _MASTERQUERYCLIENT_HPP_

#include "MessageSequencer.hpp"
#include "GameserverInformation.hpp"
#include "I18N.hpp"

class MasterQuery;

class MasterQueryClient : public GameserverInformation, public MessageSequencer {
private:
    MasterQueryClient(const MasterQueryClient&);
    MasterQueryClient& operator=(const MasterQueryClient&);

public:
    MasterQueryClient(I18N& i18n, MasterQuery& master_query, hostaddr_t host, hostport_t port);

public:
    MasterQuery& master_query;
    bool received;
    int tries;

    gametime_t last_update;

    void refresh();
    void timed_refresh(gametime_t& now);

    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version);
};

#endif
