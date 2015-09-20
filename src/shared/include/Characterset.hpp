#ifndef CHARACTERSET_HPP
#define CHARACTERSET_HPP

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

#endif // CHARACTERSET_HPP
