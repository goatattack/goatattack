#ifndef MOVABLE_HPP
#define MOVABLE_HPP

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
    const CollisionBox& get_colbox() const;
    const CollisionBox& get_damage_colbox() const;

protected:
    Subsystem& subsystem;

    int width;
    int height;
    CollisionBox colbox;
    CollisionBox damage_colbox;

    void read_base_informations(Properties& props) throw (MovableException);
    Tile *create_tile(PNG& png, int animation_speed, bool one_shot, bool desc);
    int get_speed(Properties& props, const std::string& suffix, int default_speed);
    bool get_one_shot(Properties& props, const std::string& suffix, bool default_value);
};

#endif // MOVABLE_HPP
