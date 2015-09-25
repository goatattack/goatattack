#ifndef AABB_HPP
#define AABB_HPP

struct CollisionBox {
    CollisionBox() : x(0), y(0), width(0), height(0) { }
    int x;
    int y;
    int width;
    int height;

    bool intersects(const CollisionBox& colbox) {
        if (x + width < colbox.x ||
            y + height < colbox.y ||
            x > colbox.x + colbox.width ||
            y > colbox.y + colbox.height)
        {
            return false;
        }

        return true;
    }
};

class AABB {
public:
    AABB();
    virtual ~AABB();

    const CollisionBox& get_colbox() const;

protected:
    CollisionBox colbox;
};

#endif
