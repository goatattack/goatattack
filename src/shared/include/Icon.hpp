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

#ifndef _ICON_HPP_
#define _ICON_HPP_

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

class IconException : public Exception {
public:
    IconException(const char *msg) : Exception(msg) { }
    IconException(const std::string& msg) : Exception(msg) { }
};

class Icon : public Properties {
private:
    Icon(const Icon&);
    Icon& operator=(const Icon&);

public:
    Icon(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, IconException);
    virtual ~Icon();

    Tile *get_tile();
    int get_hotspot_x() const;
    int get_hotspot_y() const;

private:
    Subsystem& subsystem;
    Tile *tile;
    int hotspot_x;
    int hotspot_y;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif
