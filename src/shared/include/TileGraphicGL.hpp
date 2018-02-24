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

#ifndef _TILEGRAPHICGL_HPP_
#define _TILEGRAPHICGL_HPP_

#include "TileGraphic.hpp"

#include <deque>
#ifdef __unix__
#include <SDL2/SDL_opengl.h>
#elif _WIN32
#include <SDL_opengl.h>
#endif

class TileGraphicGL : public TileGraphic {
public:
    TileGraphicGL(int width, int height, bool keep_pictures);
    ~TileGraphicGL();

    virtual void reset();
    virtual void add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear = false);
    virtual void replace_tile(int index, int bytes_per_pixel, const void *pic);
    virtual void punch_out_tile(PNG& png, int tilex, int tiley, bool desc, bool linear = false);
    virtual bool punch_out_lightmap(PNG& png, int tilex, int tiley);
    virtual size_t get_tile_count();

    GLuint get_texture();
    GLuint get_texture(int index);
    unsigned char *get_picture_array(int index);
    int get_bytes_per_pixel(int index);

private:
    struct PictureData {
        PictureData(int bytes_per_pixel, unsigned char *pic)
            : bytes_per_pixel(bytes_per_pixel), pic(pic) { }

        int bytes_per_pixel;
        unsigned char *pic;
    };

    typedef std::deque<GLuint> Textures;
    typedef std::deque<PictureData> Pictures;

    Textures textures;
    Pictures pictures;
    size_t sz;

    void clear();
};

#endif
