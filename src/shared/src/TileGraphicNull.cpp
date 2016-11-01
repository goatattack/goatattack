/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TileGraphicNull.hpp"

TileGraphicNull::TileGraphicNull(int width, int height, bool keep_pictures)
    : TileGraphic(width, height, keep_pictures), sz(0) { }

TileGraphicNull::~TileGraphicNull() { }

void TileGraphicNull::reset() {
    current_index = 0;
}

void TileGraphicNull::add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear) {
    sz++;
}

void TileGraphicNull::replace_tile(int index, int bytes_per_pixel, const void *pic) throw (TileGraphicException) { }

void TileGraphicNull::punch_out_tile(PNG& png, int tilex, int tiley, bool desc, bool linear) {
    add_tile(0, 0, false, linear);
}

bool TileGraphicNull::punch_out_lightmap(PNG& png, int tilex, int tiley) {
    return true;
}

size_t TileGraphicNull::get_tile_count() {
    return sz;
}
