#include "Object.hpp"

#include <cstdlib>

Object::Object(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, ObjectException)
    : Properties(filename + ".object", zip), subsystem(subsystem), tile(0),
        animation_counter(0.0f), index(0)
{
    try {
        int type_val = atoi(get_value("type").c_str());
        type = static_cast<ObjectType>(type_val);
        points = atoi(get_value("points").c_str());

        tile_width = atoi(get_value("width").c_str());
        tile_height = atoi(get_value("height").c_str());
        animation_speed = atoi(get_value("speed").c_str());
        spawning_time = atoi(get_value("spawn").c_str());
        springiness = atof(get_value("springiness").c_str());

        physics = (atoi(get_value("physics").c_str()) != 0 ? true : false);
        is_physics_colbox = (atoi(get_value("physics_colbox").c_str()) != 0 ? true : false);

        colbox.x = atoi(get_value("colbox_x").c_str());
        colbox.y = atoi(get_value("colbox_y").c_str());
        colbox.width = atoi(get_value("colbox_width").c_str());
        colbox.height = atoi(get_value("colbox_height").c_str());

        physics_colbox.x = atoi(get_value("physics_colbox_x").c_str());
        physics_colbox.y = atoi(get_value("physics_colbox_y").c_str());
        physics_colbox.width = atoi(get_value("physics_colbox_width").c_str());
        physics_colbox.height = atoi(get_value("physics_colbox_height").c_str());

        spawnable = (atoi(get_value("spawnable").c_str()) != 0 ? true : false);

        if (tile_width != tile_height || tile_width < 16 || tile_width > 32) {
            throw ObjectException("Malformed tile size: " + filename);
        }
        create_tile(filename + ".png", zip);
    } catch (const ObjectException&) {
        throw;
    } catch (const Exception& e) {
        throw ObjectException(e.what());
    }
}

Object::~Object() {
    cleanup();
}

Tile *Object::get_tile() {
    return tile;
}

int Object::get_spawning_time() const {
    return spawning_time;
}

Object::ObjectType Object::get_type() const {
    return type;
}
int Object::get_points() const {
    return points;
}

bool Object::get_physics() const {
    return physics;
}

bool Object::has_physics_colbox() const {
    return is_physics_colbox;
}

const CollisionBox& Object::get_physics_colbox() const {
    return physics_colbox;
}

double Object::get_springiness() const {
    return springiness;
}

bool Object::is_spawnable() const {
    return spawnable;
}

void Object::create_tile(const std::string& filename, ZipReader *zip) throw (Exception) {
    try {
        bool background = (atoi(get_value("background").c_str()) == 0 ? false : true);
        bool one_shot = (atoi(get_value("one_shot").c_str()) != 0 ? true : false);

        PNG png(filename, zip);

        const int& png_width = png.get_width();
        const int& png_height = png.get_height();

        int tiley = 0;
        int tilex = 0;

        /* create all pictures consecutively for this tile */
        TileGraphic *tg = subsystem.create_tilegraphic(tile_width, tile_height);
        while (tiley < png_height) {
            /* store current picture in the graphics layer */
            tg->punch_out_tile(png, tilex, tiley, false);

            /* advance to next tile */
            tilex += tile_width;
            if (tilex >= png_width) {
                tilex = 0;
                tiley += tile_height;
            }
        }

        /* create tile with its pictures */
        tile = new Tile(tg, background, Tile::TileTypeNonblocking, animation_speed, one_shot, 0.0f);
    } catch (const Exception&) {
        cleanup();
        throw;
    }
}

void Object::cleanup() {
    delete tile;
}
