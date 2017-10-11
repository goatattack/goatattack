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

#include "NPC.hpp"
#include "Globals.hpp"

#include <cstdlib>

NPC::NPC(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    : Properties(filename + ".npc", zip), Movable(subsystem), MovableColbox(subsystem)
{
    try {
        Movable::read_base_informations(*this);
        MovableColbox::read_base_informations(*this);

        move_init = atoi(get_value("move_init").c_str());
        if (move_init == 0) {
            move_init = 50;
        }

        jump_x_impulse = atof(get_value("jump_x_impulse").c_str());
        if (jump_x_impulse < Epsilon) {
            jump_x_impulse = 1.5f;
        }

        jump_y_impulse = atof(get_value("jump_y_impulse").c_str());
        if (jump_y_impulse < Epsilon) {
            jump_y_impulse = 1.5f;
        }

        max_accel_x = atof(get_value("max_accel_x").c_str());
        if (max_accel_x < Epsilon) {
            max_accel_x = 2.5f;
        }

        springiness_x = atof(get_value("springiness_x").c_str());
        springiness_y = atof(get_value("springiness_y").c_str());

        friction_factor = atof(get_value("friction_factor").c_str());
        if (friction_factor < Epsilon) {
            friction_factor = 1.0f;
        }

        idle1_counter = static_cast<double>(atoi(get_value("idle1_counter").c_str()));
        idle2_counter = static_cast<double>(atoi(get_value("idle2_counter").c_str()));

        impact = atof(get_value("impact").c_str());

        ignore_owner_counter = atof(get_value("ignore_owner_counter").c_str());
        if (ignore_owner_counter < 100.0f) {
            ignore_owner_counter = 100.0f;
        }

        create_npc(NPCAnimationStanding, filename + "_standing.png", get_speed(*this, "standing", 30), get_one_shot(*this, "standing", false), zip);
        create_npc(NPCAnimationIdle1, filename + "_idle1.png", get_speed(*this, "idle1", 30), get_one_shot(*this, "idle1", false), zip);
        create_npc(NPCAnimationIdle2, filename + "_idle2.png", get_speed(*this, "idle2", 30), get_one_shot(*this, "idle2", false), zip);
        create_npc(NPCAnimationJumping, filename + "_jumping.png", get_speed(*this, "jumping", 30), get_one_shot(*this, "jumping", false), zip);
    } catch (const Exception& e) {
        throw MovableException(std::string(e.what()) + ": " + filename);
    }
}

NPC::~NPC() {
    cleanup();
}

Tile *NPC::get_tile(Direction direction, NPCAnimation animation) {
    return tiles[direction][animation];
}

int NPC::get_move_init() const {
    return move_init;
}

int NPC::get_move_init_randomized() const {
    return (rand() % move_init + move_init);
}

double NPC::get_jump_x_impulse() const {
    return jump_x_impulse;
}

double NPC::get_jump_y_impulse() const {
    return jump_y_impulse;
}

double NPC::get_jump_x_impulse_randomized() const {
    return (rand() % static_cast<int>(jump_x_impulse * 100)) / 100.0f + jump_x_impulse;
}

double NPC::get_jump_y_impulse_randomized() const {
    return (rand() % static_cast<int>(jump_y_impulse * 100)) / 100.0f + jump_y_impulse;
}

double NPC::get_max_accel_x() const {
    return max_accel_x;
}

double NPC::get_springiness_x() const {
    return springiness_x;
}

double NPC::get_springiness_y() const {
    return springiness_y;
}

double NPC::get_friction_factor() const {
    return friction_factor;
}

double NPC::get_idle1_counter() const {
    return idle1_counter;
}

double NPC::get_idle2_counter() const {
    return idle2_counter;
}

double NPC::get_impact() const {
    return impact;
}

double NPC::get_ignore_owner_counter() const {
    return ignore_owner_counter;
}

void NPC::create_npc(NPCAnimation type, const std::string& filename,
    int animation_speed, bool one_shot, ZipReader *zip)
{
    PNG png(filename, zip);
    tiles[DirectionRight][type] = create_tile(png, animation_speed, one_shot, false);
    png.flip_h();
    tiles[DirectionLeft][type] = create_tile(png, animation_speed, one_shot, true);
}

void NPC::cleanup() {
    for (int i = 0; i < _NPCAnimationMAX; i++) {
        delete tiles[DirectionRight][i];
        delete tiles[DirectionLeft][i];
    }
}
