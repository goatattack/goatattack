#ifndef TILEGRAPHIC_HPP
#define TILEGRAPHIC_HPP

#include "PNG.hpp"

class TileGraphic {
private:
    TileGraphic(const TileGraphic&);
    TileGraphic& operator=(const TileGraphic&);

public:
    TileGraphic(int width, int height, bool keep_pictures);
    virtual ~TileGraphic();

    int get_width();
    int get_height();

    virtual void add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear = false) = 0;
    virtual void punch_out_tile(PNG& png, int tilex, int tiley, bool desc, bool linear = false) = 0;
    virtual bool punch_out_lightmap(PNG& png, int tilex, int tiley) = 0;
    virtual size_t get_tile_count() = 0;
    int& get_current_index();

protected:
    int width;
    int height;
    int current_index;
    bool keep_pictures;
};

#endif