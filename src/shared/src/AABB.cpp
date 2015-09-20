#include "AABB.hpp"

AABB::AABB() { }

AABB::~AABB() { }

const CollisionBox& AABB::get_colbox() const {
    return colbox;
}
