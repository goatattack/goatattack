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

#include "Animation.hpp"

#include <cstdlib>

Animation::Animation(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, AnimationException)
    : Properties(filename + ".animation", zip), subsystem(subsystem),
      tile(0), sound(0)
{
    try {
        tile_width = atoi(get_value("width").c_str());
        tile_height = atoi(get_value("height").c_str());
        animation_speed = atoi(get_value("speed").c_str());
        duration = atoi(get_value("duration").c_str());
        sound_loops = atoi(get_value("sound_loops").c_str());
        springiness = atof(get_value("springiness").c_str());
        impact = atof(get_value("impact").c_str());
        spread = atoi(get_value("spread").c_str());
        damage_spread = atoi(get_value("damage_spread").c_str());

        x_offset = atoi(get_value("x_offset").c_str());
        y_offset = atoi(get_value("y_offset").c_str());

        spread_count = atoi(get_value("spread_count").c_str());
        if (spread_count < 1) {
            spread_count = 1;
        }

        screen_shaker = atoi(get_value("screen_shaker").c_str());

        in_background = (atoi(get_value("background").c_str()) ? true : false);
        can_be_shot = (atoi(get_value("can_be_shot").c_str()) ? true : false);
        projectile = (atoi(get_value("projectile").c_str()) ? true : false);
        stop_sound_if_shot = (atoi(get_value("stop_sound_if_shot").c_str()) ? true : false);
        damage = atoi(get_value("damage").c_str());
        randomized_index = atoi(get_value("randomized_index").c_str());

        physics = (atoi(get_value("physics").c_str()) != 0 ? true : false);
        recoil = atof(get_value("recoil").c_str());

        physics_colbox.x = atoi(get_value("physics_colbox_x").c_str());
        physics_colbox.y = atoi(get_value("physics_colbox_y").c_str());
        physics_colbox.width = atoi(get_value("physics_colbox_width").c_str());
        physics_colbox.height = atoi(get_value("physics_colbox_height").c_str());

        damage_colbox.x = atoi(get_value("damage_colbox_x").c_str());
        damage_colbox.y = atoi(get_value("damage_colbox_y").c_str());
        damage_colbox.width = atoi(get_value("damage_colbox_width").c_str());
        damage_colbox.height = atoi(get_value("damage_colbox_height").c_str());

        if (tile_width != tile_height || tile_width < 16 || tile_width > 64) {
            throw AnimationException("Malformed tile size: " + filename);
        }
        create_tile(filename + ".png", zip);
    } catch (const AnimationException&) {
        throw;
    } catch (const Exception& e) {
        throw AnimationException(e.what());
    }
}

Animation::~Animation() {
    cleanup();
}

Tile *Animation::get_tile() const {
    return tile;
}

int Animation::get_animation_speed() const {
    return animation_speed;
}

Sound *Animation::get_sound() const {
    return sound;
}

int Animation::get_sound_loops() const {
    return sound_loops;
}

void Animation::set_sound(Sound *sound) {
    this->sound = sound;
}

bool Animation::get_physics() const {
    return physics;
}

const CollisionBox& Animation::get_physics_colbox() const {
    return physics_colbox;
}

const CollisionBox& Animation::get_damage_colbox() const {
    return damage_colbox;
}

int Animation::get_duration() const {
    return duration;
}

double Animation::get_springiness() const {
    return springiness;
}

double Animation::get_impact() const {
    return impact;
}

bool Animation::is_projectile() const {
    return projectile;
}

bool Animation::is_in_background() const {
    return in_background;
}

int Animation::get_damage() const {
    return damage;
}

int Animation::get_spread() const {
    return spread;
}

int Animation::get_spread_count() const {
    return spread_count;
}

int Animation::get_damage_spread() const {
    return damage_spread;
}

bool Animation::get_stop_sound_if_shot() const {
    return stop_sound_if_shot;
}

int Animation::get_randomized_index() const {
    if (randomized_index) {
        return rand() % randomized_index;
    } else {
        return 0;
    }
}

double Animation::get_recoil() const {
    return recoil;
}

int Animation::get_x_offset() const {
    return x_offset;
}

int Animation::get_y_offset() const {
    return y_offset;
}

int Animation::get_screen_shaker() const {
    return screen_shaker;
}

bool Animation::get_can_be_shot() const {
    return can_be_shot;
}

void Animation::create_tile(const std::string& filename, ZipReader *zip) throw (Exception) {
    try {
        int tile_type_val = atoi(get_value("tile_type").c_str());
        Tile::TileType tile_type = static_cast<Tile::TileType>(tile_type_val);

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
        tile = new Tile(tg, background, tile_type, animation_speed, one_shot, 0.0f);
    } catch (const Exception&) {
        cleanup();
        throw;
    }
}

void Animation::cleanup() {
    delete tile;
}
