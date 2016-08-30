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

#include "Lightmap.hpp"
#include "Utils.hpp"

#include <cstdlib>

Lightmap::Lightmap(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (LightmapException)
    : subsystem(subsystem), tile(0), width(0), height(0), alpha(0.85f)
{
    try {
        create_tile(filename + ".lmp", zip);
    } catch (const LightmapException&) {
        throw;
    } catch (const Exception& e) {
        throw LightmapException(e.what());
    }
}

Lightmap::~Lightmap() {
    cleanup();
}

Tile *Lightmap::get_tile(int x, int y) {
    if (x < width && y < height) {
        return tile[y][x];
    } else{
        return 0;
    }
}

void Lightmap::set_alpha(float alpha) {
    this->alpha = alpha;
}

float Lightmap::get_alpha() const {
    return alpha;
}

void Lightmap::create_tile(const std::string& filename, ZipReader *zip) throw (Exception) {
    if ((zip ? zip->file_exists(filename) : file_exists(filename))) {
        try {
            PNG png(filename, zip);
            width = static_cast<int>(png.get_width() / LightMapSize);
            height = static_cast<int>(png.get_height() / LightMapSize);
            tile = new Tile **[height];
            for (int y = 0; y < height; y++) {
                tile[y] = new Tile *[width];
                for (int x = 0; x < width; x++) {
                    tile[y][x] = 0;
                }
            }

            int tx = 0;
            int ty = 0;
            int w = width * LightMapSize;
            int h = height * LightMapSize;
            for (int y = 0; y < h; y += LightMapSize) {
                for (int x = 0; x < w; x += LightMapSize) {
                    TileGraphic *tg = subsystem.create_tilegraphic(LightMapSize, LightMapSize);
                    if (tg->punch_out_lightmap(png, x, y)) {
                        delete tg;
                    } else {
                        tile[ty][tx] = new Tile(tg, false, Tile::TileTypeNonblocking, 0, true, 0.0f);
                    }
                    tx++;
                }
                tx = 0;
                ty++;
            }
        } catch (const Exception&) {
            cleanup();
            throw;
        }
    }
}

void Lightmap::cleanup() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (tile[y][x]) {
                delete tile[y][x];
            }
        }
        delete[] tile[y];
    }
    delete[] tile;
}
