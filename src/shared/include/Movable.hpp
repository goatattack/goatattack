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

#ifndef _MOVABLE_HPP_
#define _MOVABLE_HPP_

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "AABB.hpp"
#include "Properties.hpp"

class MovableException : public Exception {
public:
    MovableException(const char *msg) : Exception(msg) { }
    MovableException(const std::string& msg) : Exception(msg) { }
};

enum Direction {
    DirectionLeft = 0,
    DirectionRight = 1,
    _DirectionMAX
};

class Movable {
public:
    Movable(Subsystem& subsystem);
    virtual ~Movable();

    int get_width() const;
    int get_height() const;

protected:
    Subsystem& subsystem;

    int width;
    int height;

    void read_base_informations(Properties& props) throw (MovableException);
    Tile *create_tile(PNG& png, int animation_speed, bool one_shot, bool desc);
    int get_speed(Properties& props, const std::string& suffix, int default_speed);
    bool get_one_shot(Properties& props, const std::string& suffix, bool default_value);
};

/* -------------------------------------------------------------------------- */
class MovableColbox {
public:
    MovableColbox(Subsystem& subsystem);
    virtual ~MovableColbox();

    const CollisionBox& get_colbox() const;
    const CollisionBox& get_damage_colbox() const;

protected:
    Subsystem& subsystem;

    CollisionBox colbox;
    CollisionBox damage_colbox;

    void read_base_informations(Properties& props) throw (MovableException);
};

#endif
