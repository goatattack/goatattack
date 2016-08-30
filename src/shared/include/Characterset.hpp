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

#ifndef _CHARACTERSET_HPP_
#define _CHARACTERSET_HPP_

#include "Properties.hpp"
#include "Movable.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

/* character icon enum */
enum CharacterAnimation {
    CharacterAnimationStanding = 0,
    CharacterAnimationRunning,
    CharacterAnimationSliding,
    CharacterAnimationJumping,
    CharacterAnimationFalling,
    CharacterAnimationLanding,
    CharacterAnimationShooting,
    _CharacterAnimationMAX
};

class Characterset : public Properties, public Movable {
private:
    Characterset(const Characterset&);
    Characterset& operator=(const Characterset&);

public:
    Characterset(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, MovableException);
    virtual ~Characterset();

    int get_flag_offset_x() const;
    int get_flag_offset_y() const;
    int get_flag_drop_offset_x() const;
    int get_flag_drop_offset_y() const;

    int get_coin_offset_x() const;
    int get_coin_offset_y() const;
    int get_coin_drop_offset_x() const;
    int get_coin_drop_offset_y() const;

    Tile *get_tile(Direction direction, CharacterAnimation animation);
    Tile *get_armor_overlay(Direction direction, CharacterAnimation animation);
    Tile *get_rifle_overlay(Direction direction, CharacterAnimation animation);
    bool get_suppress_shot_animation() const;
    int get_projectile_y_offset() const;

private:
    int flag_offset_x;
    int flag_offset_y;
    int flag_drop_offset_x;
    int flag_drop_offset_y;

    int coin_offset_x;
    int coin_offset_y;
    int coin_drop_offset_x;
    int coin_drop_offset_y;

    Tile *tiles[_DirectionMAX][_CharacterAnimationMAX];
    Tile *armor_overlays[_DirectionMAX][_CharacterAnimationMAX];
    Tile *rifle_overlays[_DirectionMAX][_CharacterAnimationMAX];
    bool suppress_shot_animation;
    int projectile_y_offset;

    void create_character(CharacterAnimation type, const std::string& filename,
        int animation_speed, bool one_shot, ZipReader *zip) throw (Exception);

    void create_armor_overlay(CharacterAnimation type, const std::string& filename,
        int animation_speed, bool one_shot, ZipReader *zip) throw (Exception);

    void create_rifle_overlay(CharacterAnimation type, const std::string& filename,
        int animation_speed, bool one_shot, ZipReader *zip) throw (Exception);

    void cleanup();
};

#endif
