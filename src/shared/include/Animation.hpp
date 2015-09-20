#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "Sound.hpp"
#include "AABB.hpp"
#include "ZipReader.hpp"

class AnimationException : public Exception {
public:
    AnimationException(const char *msg) : Exception(msg) { }
    AnimationException(const std::string& msg) : Exception(msg) { }
};

class Animation : public Properties {
public:
    Animation(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
        throw (KeyValueException, AnimationException);
    virtual ~Animation();

    Tile *get_tile() const;
    int get_animation_speed() const;
    Sound *get_sound() const;
    int get_sound_loops() const;
    void set_sound(Sound *sound);
    bool get_physics() const;
    const CollisionBox& get_physics_colbox() const;
    int get_duration() const;
    double get_springiness() const;
    double get_impact() const;
    bool is_projectile() const;
    int get_damage() const;
    int get_spread() const;
    int get_spread_count() const;
    int get_randomized_index() const;
    double get_recoil() const;
    int get_x_offset() const;
    int get_y_offset() const;
    int get_screen_shaker() const;

private:
    Subsystem& subsystem;
    Tile *tile;
    Sound *sound;
    int sound_loops;
    bool physics;
    CollisionBox physics_colbox;
    int tile_width;
    int tile_height;
    int animation_speed;
    int duration;
    double springiness;
    double impact;
    int spread;
    int spread_count;
    int randomized_index;
    bool projectile;
    int damage;
    double recoil;
    int x_offset;
    int y_offset;
    int screen_shaker;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif // ANIMATION_HPP
