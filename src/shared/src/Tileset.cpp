#include "Tileset.hpp"
#include "PNG.hpp"
#include "Globals.hpp"

#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cmath>

Tileset::Tileset(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, TilesetException)
    : Properties(filename + ".tileset", zip), subsystem(subsystem)
{
    try {
        tile_width = atoi(get_value("width").c_str());
        tile_height = atoi(get_value("height").c_str());
        if (tile_width != tile_height || tile_width < 16 || tile_width > static_cast<int>(sizeof(mask_t) * 8)) {
            throw TilesetException("Malformed tile size: " + filename);
        }
        create_tile(filename + ".png", zip);
    } catch (const TilesetException&) {
        throw;
    } catch (const Exception& e) {
        throw TilesetException(e.what());
    }
}

Tileset::~Tileset() {
    cleanup();
}

int Tileset::get_tile_width() {
    return tile_width;
}

int Tileset::get_tile_height() {
    return tile_height;
}

Tile *Tileset::get_tile(int index) {
    if (index >= 0 && index < sz) {
        return tiles[index];
    }

    return 0;
}

size_t Tileset::get_tile_count() {
    return sz;
}

void Tileset::create_tile(const std::string& filename, ZipReader *zip) throw (Exception) {
    try {
        PNG png(filename, zip);

        const int& png_width = png.get_width();
        const int& png_height = png.get_height();

        int tileno = 0;
        int tiley = 0;
        int tilex = 0;
        char kvb[128];
        while (true) {
            /* get tile properties */
            sprintf(kvb, "frames%d", tileno);
            int frames = atoi(get_value(kvb).c_str());
            if (!frames) frames = 1;

            sprintf(kvb, "tiletype%d", tileno);
            int tile_type_val = atoi(get_value(kvb).c_str());
            Tile::TileType tile_type = static_cast<Tile::TileType>(tile_type_val);

            sprintf(kvb, "background%d", tileno);
            bool background = true;
            if (get_value(kvb).length()) {
                background = (atoi(get_value(kvb).c_str()) == 0 ? false : true);
            }

            sprintf(kvb, "animation_speed%d", tileno);
            int animation_speed = atoi(get_value(kvb).c_str());
            if (!animation_speed) {
                animation_speed = 45;
            }

            sprintf(kvb, "friction%d", tileno);
            double friction = atof(get_value(kvb).c_str());
            if (friction > -Epsilon && friction < Epsilon) {
                friction = 0.05f;
            }

            sprintf(kvb, "light_blocking%d", tileno);
            bool light_blocking = false;
            if (get_value(kvb).length()) {
                light_blocking = (atoi(get_value(kvb).c_str()) != 0 ? true : false);
            }

            /* create all pictures consecutively for this tile */
            TileGraphic *tg = subsystem.create_tilegraphic(tile_width, tile_height);
            while (frames && tiley < png_height) {
                /* store current picture in the graphics layer */
                tg->punch_out_tile(png, tilex, tiley, false);

                /* advance to next tile */
                frames--;
                tilex += tile_width;
                if (tilex >= png_width) {
                    tilex = 0;
                    tiley += tile_height;
                }
            }

            /* create tile with its pictures */
            Tile *tile = new Tile(tg, background, tile_type, animation_speed, false, friction);
            tile->set_light_blocking(light_blocking);
            tiles.push_back(tile);
            sz = static_cast<int>(tiles.size());

            /* test if all pics in tileset are grabbed */
            if (tiley >= png_height) {
                break;
            }

            /* increment tileno */
            tileno++;
        }
    } catch (const Exception&) {
        cleanup();
        throw;
    }
}

void Tileset::cleanup() {
    /* delete tiles */
    for (Tiles::iterator it = tiles.begin(); it != tiles.end(); it++) {
        delete *it;
    }
}
