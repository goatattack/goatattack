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

#ifndef _MAP_HPP_
#define _MAP_HPP_

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tileset.hpp"
#include "Background.hpp"
#include "Object.hpp"
#include "Lightmap.hpp"
#include "ZipReader.hpp"

class MapException : public Exception {
public:
    MapException(const char *msg) : Exception(msg) { }
    MapException(const std::string& msg) : Exception(msg) { }
};

class Map : public Properties {
private:
    Map& operator=(const Map& rhs);

public:
    Map(Subsystem& subsystem);
    Map(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0);
    Map(const Map& rhs);
    virtual ~Map();

    const std::string& get_tileset() const;
    const std::string& get_background() const;
    int get_width();
    int get_height();
    short **get_map();
    short **get_decoration();
    Lightmap *get_lightmap();
    short get_map_tile(int y, int x);
    int get_parallax_shift() const;
    double get_decoration_brightness() const;
    double get_lightmap_alpha() const;
    void create_lightmap();
    Tile *get_preview() const;
    GamePlayType get_game_play_type() const;
    int get_frog_spawn_init() const;

protected:
    Subsystem& subsystem;
    std::string filename;
    std::string tileset;
    std::string background;
    int width;
    int height;
    int parallax;
    double lightmap_alpha;
    double decoration_brightness;
    Lightmap *lightmap;
    short **map;
    short **decoration;
    Tile *preview;
    Tile *preview_reference;
    GamePlayType game_play_type;
    int frog_spawn_init;
    std::string zip_filename;

    short **create_map(int width, int height);
    void fill_map();
    void fill_map_array(const char *prefix, short **into);
    void cleanup();
};

#endif
