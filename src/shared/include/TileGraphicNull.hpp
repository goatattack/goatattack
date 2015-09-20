#ifndef TILEGRAPHICNULL_HPP
#define TILEGRAPHICNULL_HPP

#include "TileGraphic.hpp"

class TileGraphicNull : public TileGraphic {
public:
    TileGraphicNull(int width, int height, bool keep_pictures);
    virtual ~TileGraphicNull();

    virtual void add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear = false);
    virtual void punch_out_tile(PNG& png, int tilex, int tiley, bool desc, bool linear = false);
    virtual bool punch_out_lightmap(PNG& png, int tilex, int tiley);
    virtual size_t get_tile_count();

private:
    size_t sz;
};

#endif // TILEGRAPHICNULL_HPP
