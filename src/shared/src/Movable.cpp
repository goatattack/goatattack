#include "Movable.hpp"

#include <cstdlib>

Movable::Movable(Subsystem& subsystem) : subsystem(subsystem) { }

Movable::~Movable() { }


int Movable::get_width() const {
    return width;
}

int Movable::get_height() const {
    return height;
}

const CollisionBox& Movable::get_colbox() const {
    return colbox;
}

const CollisionBox& Movable::get_damage_colbox() const {
    return damage_colbox;
}

void Movable::read_base_informations(Properties& props) throw (MovableException) {
    width = atoi(props.get_value("width").c_str());
    height = atoi(props.get_value("height").c_str());

    if (width != height || width < 16 || width > 64) {
        throw MovableException("Malformed tile size.");
    }

    colbox.x = atoi(props.get_value("colbox_x").c_str());
    colbox.y = atoi(props.get_value("colbox_y").c_str());
    colbox.width = atoi(props.get_value("colbox_width").c_str());
    colbox.height = atoi(props.get_value("colbox_height").c_str());

    damage_colbox.x = atoi(props.get_value("damage_colbox_x").c_str());
    damage_colbox.y = atoi(props.get_value("damage_colbox_y").c_str());
    damage_colbox.width = atoi(props.get_value("damage_colbox_width").c_str());
    damage_colbox.height = atoi(props.get_value("damage_colbox_height").c_str());
}

Tile *Movable::create_tile(PNG& png, int animation_speed, bool one_shot, bool desc) {
    unsigned int x = 0;
    unsigned int png_width = png.get_width();
    TileGraphic *tg = subsystem.create_tilegraphic(width, height);
    while (x < png_width) {
        tg->punch_out_tile(png, x, 0, desc);
        x += width;
    }
    return new Tile(tg, false, Tile::TileTypeBlocking, animation_speed, one_shot, 0.0f);
}


int Movable::get_speed(Properties& props, const std::string& suffix, int default_speed) {
    const std::string& speed_str = props.get_value("animation_speed_" + suffix);
    int animation_speed = 0;
    if (speed_str.length()) {
        animation_speed = atoi(speed_str.c_str());
    } else {
        animation_speed = default_speed;
    }

    return animation_speed;
}

bool Movable::get_one_shot(Properties& props, const std::string& suffix, bool default_value) {
    const std::string& one_shot_str = props.get_value("one_shot_" + suffix);
    bool one_shot = false;
    if (one_shot_str.length()) {
        one_shot = (atoi(one_shot_str.c_str()) != 0 ? true : false);
    } else {
        one_shot = default_value;
    }

    return one_shot;
}
