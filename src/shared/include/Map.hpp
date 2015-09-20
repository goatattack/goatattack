#ifndef MAP_HPP
#define MAP_HPP

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tileset.hpp"
#include "Background.hpp"
#include "Object.hpp"
#include "Lightmap.hpp"
#include "ZipReader.hpp"

class MapException : public Exception {
public:
    MapException(const char *msg) : Exception(msg) { }
    MapException(const std::string& msg) : Exception(msg) { }
};

class Map : public Properties {
private:
    Map& operator=(const Map& rhs);

public:
    Map(Subsystem& subsystem);
    Map(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, MapException);
    Map(const Map& rhs);
    virtual ~Map();

    const std::string& get_tileset() const;
    const std::string& get_background() const;
    int get_width();
    int get_height();
    short **get_map();
    short **get_decoration();
    Lightmap *get_lightmap();
    short get_map_tile(int y, int x);
    int get_parallax_shift() const;
    double get_decoration_brightness() const;
    double get_lightmap_alpha() const;
    void create_lightmap();
    Tile *get_preview();
    GamePlayType get_game_play_type() const;
    int get_frog_spawn_init() const;

protected:
    Subsystem& subsystem;
    std::string filename;
    std::string tileset;
    std::string background;
    int width;
    int height;
    int parallax;
    double lightmap_alpha;
    double decoration_brightness;
    Lightmap *lightmap;
    short **map;
    short **decoration;
    Tile *preview;
    GamePlayType game_play_type;
    int frog_spawn_init;
    std::string zip_filename;

    short **create_map(int width, int height);
    void fill_map() throw (Exception);
    void fill_map_array(const char *prefix, short **into);
    void cleanup();
};

#endif // MAP_HPP
