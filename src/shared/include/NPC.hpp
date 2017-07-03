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

#ifndef _NPC_HPP_
#define _NPC_HPP_

#include "Properties.hpp"
#include "Movable.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

/* NPC icon enum */
enum NPCAnimation {
    NPCAnimationStanding = 0,
    NPCAnimationIdle1,
    NPCAnimationIdle2,
    NPCAnimationJumping,
    _NPCAnimationMAX
};

class NPC : public Properties, public Movable, public MovableColbox {
private:
    NPC(const NPC&);
    NPC& operator=(const NPC&);

public:
    NPC(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, MovableException);
    virtual ~NPC();

    Tile *get_tile(Direction direction, NPCAnimation animation);
    int get_move_init() const;
    int get_move_init_randomized() const;
    double get_jump_x_impulse() const;
    double get_jump_y_impulse() const;
    double get_jump_x_impulse_randomized() const;
    double get_jump_y_impulse_randomized() const;
    double get_max_accel_x() const;
    double get_springiness_x() const;
    double get_springiness_y() const;
    double get_friction_factor() const;
    double get_idle1_counter() const;
    double get_idle2_counter() const;
    double get_impact() const;
    double get_ignore_owner_counter() const;

private:
    Tile *tiles[_DirectionMAX][_NPCAnimationMAX];
    int move_init;
    double jump_x_impulse;
    double jump_y_impulse;
    double max_accel_x;
    double springiness_x;
    double springiness_y;
    double friction_factor;
    double idle1_counter;
    double idle2_counter;
    double impact;
    double ignore_owner_counter;

    void create_npc(NPCAnimation type, const std::string& filename,
        int animation_speed, bool one_shot, ZipReader *zip) throw (Exception);

    void cleanup();
};

#endif
