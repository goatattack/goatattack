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

#include "Tile.hpp"

const char *Tile::TypeDescription[] = {
    "non blocking",
    "blocking",
    "falling only blocking",
    "red base",
    "blue base",
    "killing",
    "jump, no descend",
    "speed race finish",
    "hill zone"
};

Tile::Tile(TileGraphic *tilegraphic, bool background, TileType tile_type,
    int animation_speed, bool one_shot, double friction)
    : background(background), tile_type(tile_type),
      animation_speed(animation_speed),
      tilegraphic(tilegraphic), one_shot(one_shot), friction(friction),
      animation_counter(0.0f), light_blocking(false) { }

Tile::~Tile() {
    delete tilegraphic;
}

TileGraphic *Tile::get_tilegraphic() {
    return tilegraphic;
}

bool Tile::is_background() {
    return background;
}

Tile::TileType Tile::get_tile_type() {
    return tile_type;
}

int Tile::get_animation_speed() {
    return animation_speed;
}

bool Tile::is_one_shot() {
    return one_shot;
}

double Tile::get_friction() {
    return friction;
}

void Tile::update_tile_index(double diff) {
    int& current_index = tilegraphic->get_current_index();
    animation_counter += diff;
    if (animation_counter > static_cast<double>(animation_speed)) {
        animation_counter = 0.0f;
        current_index++;
        if (current_index >= static_cast<int>(tilegraphic->get_tile_count())) {
            current_index = 0;
        }
    }
}

void Tile::set_is_background(bool state) {
    background = state;
}

void Tile::set_animation_speed(int speed) {
    animation_speed = speed;
}

void Tile::set_friction(double friction) {
    this->friction = friction;
}

void Tile::set_type(TileType type) {
    tile_type = type;
}

bool Tile::is_light_blocking() const {
    return light_blocking;
}

void Tile::set_light_blocking(bool state) {
    light_blocking = state;
}
