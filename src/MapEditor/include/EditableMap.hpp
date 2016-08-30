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

#ifndef _EDITABLEMAP_HPP_
#define _EDITABLEMAP_HPP_

#include "Resources.hpp"
#include "Subsystem.hpp"
#include "EditableObject.hpp"
#include "EditableLight.hpp"

#include <string>

class EditableMap : public Map {
public:
    typedef std::vector<EditableObject *> Objects;
    typedef std::vector<EditableLight *> Lights;

    EditableMap(Resources& resources, Subsystem& subsystem) throw (Exception);
    EditableMap(Resources& resources, Subsystem& subsystem, const std::string& filename) throw (Exception);
    virtual ~EditableMap();

    Tileset *get_tileset_ptr() const;
    Background *get_background_ptr() const;

    Objects& get_objects();
    Lights& get_light_sources();

    void set_parallax_shift(int parallax);
    void set_decoration_brightness(double brightness);
    void set_lightmap_alpha(double alpha);
    void set_tileset(const std::string& tileset);
    void set_background(const std::string& background);
    void resize_map(int new_width, int new_height);
    void set_tile(int x, int y, short index);
    void set_decoration(int x, int y, short index);
    void set_object(Object *obj, int x, int y);
    void set_game_play_type(GamePlayType type);
    void set_frog_spawn_init(int t);
    void erase_object(int x, int y);
    void set_light(int x, int y);
    void erase_light(int x, int y);
    void save() throw (Exception);

private:
    Resources& resources;
    Subsystem& subsystem;
    Tileset *ptileset;
    Background *pbackground;
    Objects objects;
    Lights lights;

    short **create_new_array(short **from, int new_width, int new_height);
    void save_array(const char *prefix, short **from, int width, int height);
};

#endif
