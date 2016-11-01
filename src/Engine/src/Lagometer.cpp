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

#include "Lagometer.hpp"

#include <cstdlib>

static const int BytesPerPixel = 4;
static const int IntervalInMs = 500;
static const ms_t MaxPing = 100;

Lagometer::Lagometer(Subsystem& subsystem) throw (LagometerException)
    : meter(0), width(60), height(16), tpic(new unsigned char[width * height * BytesPerPixel]),
      max_ping(0)
{
    create(subsystem);
    get_now(last_update);
}

Lagometer::Lagometer(Subsystem& subsystem, int width, int height) throw (LagometerException)
    : meter(0), width(width), height(height), tpic(new unsigned char[width * height * BytesPerPixel]),
      max_ping(0)
{
    create(subsystem);
    get_now(last_update);
}

Lagometer::~Lagometer() {
    delete meter;
}

void Lagometer::update(ms_t ping) {
    if (ping > max_ping) {
        max_ping = ping;
    }
}

void Lagometer::clear() {
    /* fill vector with zeros */
    pings.clear();
    for (int w = 0; w < width; w++) {
        pings.push_back(0);
    }
}

TileGraphic *Lagometer::get_tilegraphic() {
    gametime_t now;
    get_now(now);

    if (diff_ms(last_update, now) >= IntervalInMs) {
        /* update time and ping vector */
        last_update = now;
        pings.erase(pings.begin());
        ms_t ping = (max_ping < MaxPing ? max_ping : MaxPing);
        ping = height * ping / MaxPing;
        pings.push_back(ping);
        max_ping = 0;

        /* update picture */
        for (int x = 0; x < width; x++) {
            int current_value = pings[x];
            unsigned char colr = (static_cast<double>(current_value) / height * 255);
            unsigned char colg = 255 - colr;
            for (int y = 0; y < height; y++) {
                unsigned char *pptr = &tpic[((height - 1) - y) * width * BytesPerPixel + x * BytesPerPixel];
                if (y < current_value) {
                    pptr[0] = colr; pptr[1] = colg; pptr[2] = 0; pptr[3] = 255;
                } else {
                    pptr[0] = 0; pptr[1] = 0; pptr[2] = 0; pptr[3] = 255;
                }
            }
        }
        meter->replace_tile(0, BytesPerPixel, &tpic[0]);
    }

    return meter;
}

void Lagometer::create(Subsystem& subsystem) throw (LagometerException) {
    clear();

    /* clear picture */
    unsigned char *pptr = &tpic[0];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            *pptr++ = 0; *pptr++ = 0; *pptr++ = 0; *pptr++ = 255;
        }
    }

    AutoPtr<TileGraphic> tmeter(subsystem.create_tilegraphic(width, height));
    tmeter->add_tile(BytesPerPixel, &tpic[0], false, false);
    meter = tmeter.release();
}