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

#ifndef _LAGOMETER_HPP_
#define _LAGOMETER_HPP_

#include "TileGraphic.hpp"
#include "Exception.hpp"
#include "Subsystem.hpp"
#include "AutoPtr.hpp"
#include "Timing.hpp"

#include <vector>

class LagometerException : public Exception {
public:
    LagometerException(const char *msg) : Exception(msg) { }
    LagometerException(std::string msg) : Exception(msg) { }
};

class Lagometer {
private:
     Lagometer(const Lagometer&);
     Lagometer& operator=(const Lagometer&);

public:
    Lagometer(Subsystem& subsystem) throw (LagometerException);
    Lagometer(Subsystem& subsystem, int width, int height) throw (LagometerException);
    ~Lagometer();

    void update(ms_t ping, int server_outq_sz, int client_outq_sz);
    void clear();
    TileGraphic *get_tilegraphic();

private:
    struct LagInfo {
        LagInfo(ms_t ping, int server_outq_sz, int client_outq_sz)
            : ping(ping), server_outq_sz(server_outq_sz), client_outq_sz(client_outq_sz) { }

        ms_t ping;
        int server_outq_sz;
        int client_outq_sz;
    };

    typedef std::vector<LagInfo> Pings;

    TileGraphic *meter;
    int width;
    int height;
    int height2;
    int height4;
    int pictop;
    AutoPtr<unsigned char[]> tpic;
    AutoPtr<unsigned char[]> red;
    AutoPtr<unsigned char[]> green;
    ms_t max_ping;
    int max_server_outq_sz;
    int max_client_outq_sz;
    gametime_t last_update;

    Pings pings;

    void create(Subsystem& subsystem) throw (LagometerException);
};

#endif