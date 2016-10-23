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

#ifndef _TILE_HPP_
#define _TILE_HPP_

#include "TileGraphic.hpp"
#include "I18N.hpp"

#include <stdint.h>

typedef uint64_t mask_t;

class Tile {
private:
    Tile(const Tile&);
    Tile& operator=(const Tile&);

public:
    static const I18NText TypeDescription[];

    enum TileType {
        TileTypeNonblocking = 0,
        TileTypeBlocking,
        TileTypeFallingOnlyBlocking,
        TileTypeBaseRed,
        TileTypeBaseBlue,
        TileTypeKilling,
        TileTypeFallingOnlyBlockingNoDescending,
        TileTypeSpeedraceFinish,
        TileTypeHillZone,
        _TileTypeMAX
    };

    Tile(TileGraphic *tilegraphic, bool background, TileType tile_type,
        int animation_speed, bool one_shot, double friction);
    virtual ~Tile();

    TileGraphic *get_tilegraphic();
    bool is_background();
    TileType get_tile_type();
    int get_animation_speed();
    bool is_one_shot();
    double get_friction();
    void update_tile_index(double diff);

    void set_is_background(bool state);
    void set_animation_speed(int speed);
    void set_friction(double friction);
    void set_type(TileType type);
    bool is_light_blocking() const;
    void set_light_blocking(bool state);

private:
    bool background;
    TileType tile_type;
    int animation_speed;
    TileGraphic *tilegraphic;
    bool one_shot;
    double friction;
    double animation_counter;
    bool light_blocking;
};

#endif
