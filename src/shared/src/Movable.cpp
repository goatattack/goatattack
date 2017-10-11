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

#include "Movable.hpp"

#include <cstdlib>

/* -------------------------------------------------------------------------- */
Movable::Movable(Subsystem& subsystem) : subsystem(subsystem) { }

Movable::~Movable() { }

int Movable::get_width() const {
    return width;
}

int Movable::get_height() const {
    return height;
}

void Movable::read_base_informations(Properties& props) throw (MovableException) {
    width = atoi(props.get_value("width").c_str());
    height = atoi(props.get_value("height").c_str());

    if (width != height || width < 16 || width > 64) {
        throw MovableException("Malformed tile size.");
    }
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

/* -------------------------------------------------------------------------- */

MovableColbox::MovableColbox(Subsystem& subsystem) : subsystem(subsystem) { }

MovableColbox::~MovableColbox() { }

const CollisionBox& MovableColbox::get_colbox() const {
    return colbox;
}

const CollisionBox& MovableColbox::get_damage_colbox() const {
    return damage_colbox;
}

void MovableColbox::read_base_informations(Properties& props) {
    colbox.x = atoi(props.get_value("colbox_x").c_str());
    colbox.y = atoi(props.get_value("colbox_y").c_str());
    colbox.width = atoi(props.get_value("colbox_width").c_str());
    colbox.height = atoi(props.get_value("colbox_height").c_str());

    damage_colbox.x = atoi(props.get_value("damage_colbox_x").c_str());
    damage_colbox.y = atoi(props.get_value("damage_colbox_y").c_str());
    damage_colbox.width = atoi(props.get_value("damage_colbox_width").c_str());
    damage_colbox.height = atoi(props.get_value("damage_colbox_height").c_str());
}
