#include "TileGraphicNull.hpp"

TileGraphicNull::TileGraphicNull(int width, int height, bool keep_pictures)
    : TileGraphic(width, height, keep_pictures), sz(0) { }

TileGraphicNull::~TileGraphicNull() { }

void TileGraphicNull::add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear) {
    sz++;
}

void TileGraphicNull::punch_out_tile(PNG& png, int tilex, int tiley, bool desc, bool linear) {
    add_tile(0, 0, false, linear);
}

bool TileGraphicNull::punch_out_lightmap(PNG& png, int tilex, int tiley) {
    return true;
}

size_t TileGraphicNull::get_tile_count() {
    return sz;
}
