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

#ifndef _TILESET_HPP_
#define _TILESET_HPP_

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "Properties.hpp"
#include "ZipReader.hpp"

#include <string>
#include <vector>

#ifdef _WIN32
#include "Win.hpp"
#endif

class TilesetException : public Exception {
public:
    TilesetException(const char *msg) : Exception(msg) { }
    TilesetException(const std::string& msg) : Exception(msg) { }
};

class Tileset : public Properties {
private:
    Tileset(const Tileset&);
    Tileset& operator=(const Tileset&);

public:
    Tileset(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, TilesetException);
    virtual ~Tileset();

    int get_tile_width();
    int get_tile_height();
    Tile *get_tile(int index);
    size_t get_tile_count();
    bool is_hidden_in_mapeditor() const;

private:
    typedef std::vector<Tile *> Tiles;

    Subsystem& subsystem;

    int tile_width;
    int tile_height;
    bool hidden_in_mapeditor;
    Tiles tiles;
    int sz;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif
