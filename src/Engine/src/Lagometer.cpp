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

Lagometer::Lagometer(Subsystem& subsystem)
    : meter(0), width(32), height(32), height2(height / 2), height4(height / 4),
      pictop(height - 1), tpic(new unsigned char[width * height * BytesPerPixel]),
      red(new unsigned char[height2 + 1]), green(new unsigned char[height2 + 1]),
      max_ping(0), max_server_outq_sz(0), max_client_outq_sz(0)
{
    create(subsystem);
    get_now(last_update);
}

Lagometer::Lagometer(Subsystem& subsystem, int width, int height)
    : meter(0), width(width), height(height), height2(height / 2), height4(height / 4),
      pictop(height - 1), tpic(new unsigned char[width * height * BytesPerPixel]),
      red(new unsigned char[height2 + 1]), green(new unsigned char[height2 + 1]),
      max_ping(0), max_server_outq_sz(0), max_client_outq_sz(0)
{
    create(subsystem);
    get_now(last_update);
}

Lagometer::~Lagometer() {
    delete meter;
}

void Lagometer::update(ms_t ping, int server_outq_sz, int client_outq_sz) {
    if (ping > max_ping) {
        max_ping = ping;
    }
    if (server_outq_sz > max_server_outq_sz) {
        max_server_outq_sz = server_outq_sz;
    }
    if (client_outq_sz > max_client_outq_sz) {
        max_client_outq_sz = client_outq_sz;
    }
}

void Lagometer::clear() {
    /* fill vector with zeros */
    pings.clear();
    for (int w = 0; w < width; w++) {
        pings.push_back(LagInfo(0, 0, 0));
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
        ping = height2 * ping / MaxPing;
        int srvoutq = (max_server_outq_sz > height4 ? height4 : max_server_outq_sz);
        int clioutq = (max_client_outq_sz > height4 ? height4 : max_client_outq_sz);
        pings.push_back(LagInfo(ping, srvoutq, clioutq));
        max_ping = 0;
        max_server_outq_sz = 0;
        max_client_outq_sz = 0;

        /* update picture */
        for (int x = 0; x < width; x++) {
            /* get current strip info */
            LagInfo& li = pings[x];

            /* clear strip */
            for (int y = li.ping; y < height; y++) {
                unsigned char *pptr = &tpic[(pictop - y) * width * BytesPerPixel + x * BytesPerPixel];
                pptr[0] = 0; pptr[1] = 0; pptr[2] = 0; pptr[3] = 255;
            }

            /* draw pings */
            for (int y = 0; y < li.ping; y++) {
                unsigned char *pptr = &tpic[(pictop - y) * width * BytesPerPixel + x * BytesPerPixel];
                pptr[0] = red[y]; pptr[1] = green[y]; pptr[2] = 0; pptr[3] = 255;
            }

            /* draw server outq */
            for (int y = 0; y < li.server_outq_sz; y++) {
                unsigned char *pptr = &tpic[(height4 - y - 1) * width * BytesPerPixel + x * BytesPerPixel];
                pptr[0] = 255; pptr[1] = 255; pptr[2] = 0; pptr[3] = 255;
            }

            /* draw client outq */
            for (int y = 0; y < li.client_outq_sz; y++) {
                unsigned char *pptr = &tpic[(height4 + y) * width * BytesPerPixel + x * BytesPerPixel];
                pptr[0] = 127; pptr[1] = 127; pptr[2] = 255; pptr[3] = 255;
            }
        }
        meter->replace_tile(0, BytesPerPixel, &tpic[0]);
    }

    return meter;
}

void Lagometer::create(Subsystem& subsystem) {
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

    /* create color transition */
    for (int y = 0; y <= height2; y++) {
        int v = static_cast<int>(y * 1.4);
        if (v > height2) {
            v = height2;
        }
        unsigned char colr = v * 255 / height2;
        unsigned char colg = 255 - colr;
        unsigned char diff = 255 - (colr > colg ? colr : colg);
        red[y] = colr + diff;
        green[y] = colg + diff;
    }

    /* done */
    meter = tmeter.release();
}
