#ifndef EDITABLEMAP_HPP
#define EDITABLEMAP_HPP

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

#endif // EDITABLEMAP_HPP
