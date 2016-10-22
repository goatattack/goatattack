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

#ifndef _LANBROADCASTER_HPP_
#define _LANBROADCASTER_HPP_

#include "Exception.hpp"
#include "MessageSequencer.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "GameserverInformation.hpp"
#include "I18N.hpp"

class LANBroadcasterException : public Exception {
public:
    LANBroadcasterException(const char *msg) : Exception(msg) { }
    LANBroadcasterException(std::string msg) : Exception(msg) { }
};

class LANBroadcaster : public ServerList, public MessageSequencer, public Thread {
private:
    LANBroadcaster(const LANBroadcaster&);
    LANBroadcaster& operator=(const LANBroadcaster&);

public:
    LANBroadcaster(I18N& i18n, hostport_t port) throw (LANBroadcasterException, UDPSocketException);
    virtual ~LANBroadcaster();

    void start() throw (LANBroadcasterException);
    void stop();
    void refresh() throw (Exception);
    void clear();
    Mutex& get_mutex();

private:
    I18N& i18n;
    hostport_t port;
    bool running;

    Mutex mtx;

    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version) throw (Exception);

    void cleanup();

    virtual void thread();
};

#endif
