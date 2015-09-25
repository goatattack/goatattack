#ifndef TILEGRAPHICGL_HPP
#define TILEGRAPHICGL_HPP

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
    virtual ~TileGraphicGL();

    virtual void add_tile(int bytes_per_pixel, const void *pic, bool desc, bool linear = false);
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
};

#endif