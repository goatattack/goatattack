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

#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include "Exception.hpp"
#include "Properties.hpp"
#include "AABB.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

class ObjectException : public Exception {
public:
    ObjectException(const char *msg) : Exception(msg) { }
    ObjectException(const std::string& msg) : Exception(msg) { }
};

class Object : public Properties, public AABB {
private:
    Object(const Object&);
    Object& operator=(const Object&);

public:
    enum ObjectType {
        ObjectTypeNothing = 0,      /*  0 */
        ObjectTypeSpawnPointRed,    /*  1 */
        ObjectTypeSpawnPointBlue,   /*  2 */
        ObjectTypeRedFlag,          /*  3 */
        ObjectTypeBlueFlag,         /*  4 */
        ObjectTypeBomb,             /*  5 */
        ObjectTypeArmor,            /*  6 */
        ObjectTypeFood,             /*  7 */
        ObjectTypePoints,           /*  8 */
        ObjectTypeGrenade,          /*  9 */
        ObjectTypeMedikitBig,       /* 10 */
        ObjectTypeMedikitSmall,     /* 11 */
        ObjectTypeShotgun,          /* 12 */
        ObjectTypeAmmo,             /* 13 */
        ObjectTypeAmmobox,          /* 14 */
        ObjectTypeLight,            /* 15 */
        ObjectTypeSpawnPointFrog,   /* 16 */
        ObjectTypeFrog,             /* 17 */
        ObjectTypeCoin,             /* 18 */
        _ObjectTypeMAX
    };

    Object(Subsystem& subsystem, const std::string& filename, ZipReader *zip);
    virtual ~Object();

    Tile *get_tile();
    void increment_tile_index(double diff) const;
    int get_spawning_time() const;
    ObjectType get_type() const;
    int get_points() const;
    bool get_physics() const;
    bool has_physics_colbox() const;
    const CollisionBox& get_physics_colbox() const;
    double get_springiness() const;
    bool is_spawnable() const;

private:
    Subsystem& subsystem;
    Tile *tile;
    double animation_counter;
    int index;

    int tile_width;
    int tile_height;
    ObjectType type;
    bool physics;
    bool is_physics_colbox;
    CollisionBox physics_colbox;
    int points;
    int animation_speed;
    int spawning_time;
    double springiness;
    bool spawnable;

    void create_tile(const std::string& filename, ZipReader *zip);
    void cleanup();
};

#endif
