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

#include "Map.hpp"

#include <cstdlib>

Map::Map(Subsystem& subsystem) : subsystem(subsystem) {
    parallax = 0;
    decoration_brightness = 0.0f;
    lightmap_alpha = 0.0f;
    width = 40;
    height = 22;
    map = create_map(width, height);
    decoration = create_map(width, height);
    lightmap = 0;
    preview = 0;
    game_play_type = GamePlayTypeDM;
    frog_spawn_init = 0;
}

Map::Map(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    : Properties(filename + ".map", zip), subsystem(subsystem), filename(filename),
      tileset(get_value("tileset")),
      background(get_value("background"))
{
    try {
        parallax = atoi(get_value("parallax_shift").c_str());
        decoration_brightness = atof(get_value("decoration_brightness").c_str());
        lightmap_alpha = atof(get_value("lightmap_alpha").c_str());
        if (lightmap_alpha < 0.05f) {
            lightmap_alpha = 0.85f;
        }

        game_play_type = static_cast<GamePlayType>(atoi(get_value("game_play_type").c_str()));
        frog_spawn_init = atoi(get_value("frog_spawn_init").c_str());

        /* create map array */
        width = atoi(get_value("width").c_str());
        height = atoi(get_value("height").c_str());
        map = create_map(width, height);
        decoration = create_map(width, height);
        if (zip) {
            zip_filename = zip->get_zip_filename();
        }
        fill_map();
    } catch (const MapException&) {
        throw;
    } catch (const Exception& e) {
        throw MapException(e.what());
    }
    lightmap = 0;
    preview = 0;

    try {
        PNG png(filename +".png", zip);
        int w = (png.get_width() < 64 ? png.get_width() : 64);
        int h = (png.get_height() < 64 ? png.get_height() : 64);
        TileGraphic *tg = subsystem.create_tilegraphic(w, h);
        tg->punch_out_tile(png, 0, 0, false, false);
        preview = new Tile(tg, false, Tile::TileTypeNonblocking, 0, true, 0.0f);
    } catch (const Exception&) {
        /* chomp */
    }
}

Map::Map(const Map& rhs)
    : Properties(rhs),
      subsystem(rhs.subsystem),
      filename(rhs.filename),
      tileset(rhs.tileset),
      background(rhs.background),
      width(rhs.width),
      height(rhs.height),
      parallax(rhs.parallax),
      lightmap_alpha(rhs.lightmap_alpha),
      decoration_brightness(rhs.decoration_brightness),
      lightmap(0),
      preview(0),
      game_play_type(rhs.game_play_type),
      frog_spawn_init(rhs.frog_spawn_init),
      zip_filename(rhs.zip_filename)
{
    /* copy map */
    map = new short *[height];
    for (int y = 0; y < height; y++) {
        map[y] = new short[width];
        for (int x = 0; x < width; x++) {
            map[y][x] = rhs.map[y][x];
        }
    }

    /* copy decoration */
    decoration = new short *[height];
    for (int y = 0; y < height; y++) {
        decoration[y] = new short[width];
        for (int x = 0; x < width; x++) {
            decoration[y][x] = rhs.decoration[y][x];
        }
    }
}

Map::~Map() {
    cleanup();
    if (lightmap) {
        delete lightmap;
    }
    if (preview) {
        delete preview;
    }
}

const std::string& Map::get_tileset() const {
    return tileset;
}

const std::string& Map::get_background() const {
    return background;
}

int Map::get_width() {
    return width;
}

int Map::get_height() {
    return height;
}

short **Map::get_map() {
    return map;
}

short **Map::get_decoration() {
    return decoration;
}

Lightmap *Map::get_lightmap() {
    return lightmap;
}

short Map::get_map_tile(int y, int x) {
    if (y >= 0 && y < height) {
        if (x >= 0 && x <= width) {
            return map[y][x];
        }
    }

    return -1;
}

int Map::get_parallax_shift() const {
    return parallax;
}

double Map::get_decoration_brightness() const {
    return decoration_brightness;
}

double Map::get_lightmap_alpha() const {
    return lightmap_alpha;
}

void Map::create_lightmap() {
    if (lightmap) {
        delete lightmap;
    }

    if (zip_filename.length()) {
        ZipReader zip(zip_filename);
        lightmap = new Lightmap(subsystem, filename, &zip);
    } else {
        lightmap = new Lightmap(subsystem, filename);
    }
    lightmap->set_alpha(static_cast<float>(lightmap_alpha));
}

Tile *Map::get_preview() {
    return preview;
}

GamePlayType Map::get_game_play_type() const {
    return game_play_type;
}

int Map::get_frog_spawn_init() const {
    return frog_spawn_init;
}

short **Map::create_map(int width, int height) {
    short **map = new short *[height];
    for (int y = 0; y < height; y++) {
        map[y] = new short[width];
        for (int x = 0; x < width; x++) {
            map[y][x] = -1;
        }
    }

    return map;
}

void Map::fill_map() throw (Exception) {
    fill_map_array("decoration", decoration);
    fill_map_array("tiles", map);
}

void Map::fill_map_array(const char *prefix, short **into) {
    char kvb[128];
    for (int y = 0; y < height; y++) {
        sprintf(kvb, "%s%d", prefix, y);
        std::string line = get_value(kvb);
        std::string tileno;
        int x = 0;
        while (x < width && line.length()) {
            size_t pos = line.find(',');
            if (pos == std::string::npos) {
                tileno = line;
                line.clear();
            } else {
                tileno = line.substr(0, pos);
                line = line.substr(pos + 1);
            }
            into[y][x] = atoi(tileno.c_str());
            x++;
        }
    }
}

void Map::cleanup() {
    for (int y = 0; y < height; y++) {
        delete[] map[y];
        delete[] decoration[y];
    }
    delete[] map;
    delete[] decoration;
}
