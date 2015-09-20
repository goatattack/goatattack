#ifndef TILE_HPP
#define TILE_HPP

#include "TileGraphic.hpp"

#include <stdint.h>

typedef uint64_t mask_t;

class Tile {
private:
    Tile(const Tile&);
    Tile& operator=(const Tile&);

public:
    static const char *TypeDescription[];

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

#endif // TILE_HPP
