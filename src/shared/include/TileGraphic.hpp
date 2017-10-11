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

#ifndef _TILEGRAPHIC_HPP_
#define _TILEGRAPHIC_HPP_

#include "PNG.hpp"
#include "Exception.hpp"

class TileGraphicException : public Exception {
public:
    TileGraphicException(const char *msg) : Exception(msg) { }
    TileGraphicException(std::string msg) : Exception(msg) { }
};

class TileGraphic {
private:
    TileGraphic(const TileGraphic&);
    TileGraphic& operator=(const TileGraphic&);

public:
    TileGraphic(int width, int height, bool keep_pictures);
    virtual ~TileGraphic();

    int get_width();
    int get_height();

    virtual void reset() = 0;
    virtual void add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear = false) = 0;
    virtual void replace_tile(int index, int bytes_per_pixel, const void *pic) = 0;
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
