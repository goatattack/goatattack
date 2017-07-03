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

#ifndef _MASTERQUERY_HPP_
#define _MASTERQUERY_HPP_

#include "Exception.hpp"
#include "MasterQueryClient.hpp"
#include "Timing.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "I18N.hpp"

#include <vector>

class MasterQueryException : public Exception {
public:
    MasterQueryException(const char *msg) : Exception(msg) { }
    MasterQueryException(std::string msg) : Exception(msg) { }
};

class MasterQuery : public ServerList, public Thread {
private:
    MasterQuery(const MasterQuery&);
    MasterQuery& operator=(const MasterQuery&);

public:
    MasterQuery(I18N& i18n, const std::string& masterserver, hostport_t masterport)
        throw (MasterQueryException);
    virtual ~MasterQuery();

    void start() throw (MasterQueryException);
    void stop();
    void cycle(gametime_t *now);
    void refresh() throw (MasterQueryException);
    void locked_sort();

    Mutex& get_mutex();

private:
    I18N& i18n;
    std::string masterserver;
    hostport_t masterport;
    bool running;

    Mutex mtx;

    void cleanup();

    virtual void thread();
};

#endif
