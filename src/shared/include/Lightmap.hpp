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

#ifndef _LIGHTMAP_HPP_
#define _LIGHTMAP_HPP_

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

const int LightMapSize = 128;

class LightmapException : public Exception {
public:
    LightmapException(const char *msg) : Exception(msg) { }
    LightmapException(const std::string& msg) : Exception(msg) { }
};

class Lightmap {
private:
    Lightmap(const Lightmap&);
    Lightmap& operator=(const Lightmap&);

public:
    Lightmap(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (LightmapException);
    virtual ~Lightmap();

    Tile *get_tile(int x, int y);
    void set_alpha(float alpha);
    float get_alpha() const;

private:
    Subsystem& subsystem;
    Tile ***tile;
    int width;
    int height;
    float alpha;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif
