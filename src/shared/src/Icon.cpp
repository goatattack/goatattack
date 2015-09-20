#include "Icon.hpp"

#include <cstdlib>

Icon::Icon(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, IconException)
    : Properties(filename + ".icon", zip), subsystem(subsystem), tile(0),
      hotspot_x(0), hotspot_y(0)
{
    try {
        hotspot_x = atoi(get_value("hotspot_x").c_str());
        hotspot_y = atoi(get_value("hotspot_y").c_str());
        create_tile(filename + ".png", zip);
    } catch (const IconException&) {
        throw;
    } catch (const Exception& e) {
        throw IconException(e.what());
    }
}

Icon::~Icon() {
    cleanup();
}

Tile *Icon::get_tile() {
    return tile;
}

int Icon::get_hotspot_x() const {
    return hotspot_x;
}

int Icon::get_hotspot_y() const {
    return hotspot_y;
}

void Icon::create_tile(const std::string& filename, ZipReader *zip) throw (Exception) {
    try {
        PNG png(filename, zip);
        TileGraphic *tg = subsystem.create_tilegraphic(png.get_width(), png.get_height());
        bool linear = (atoi(get_value("linear").c_str()) != 0 ? true : false);
        tg->punch_out_tile(png, 0, 0, false, linear);
        tile = new Tile(tg, false, Tile::TileTypeNonblocking, 0, true, 0.0f);
    } catch (const Exception&) {
        cleanup();
        throw;
    }
}

void Icon::cleanup() {
    delete tile;
}
