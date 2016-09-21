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

#ifndef _SUBSYSTEMSDL_HPP_
#define _SUBSYSTEMSDL_HPP_

#include "Subsystem.hpp"
#include "Shader.hpp"
#include "TileGraphicGL.hpp"

#include <vector>
#ifdef __unix__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_opengl.h>
#endif

class Resources;

class SubsystemSDL : public Subsystem {
public:
    SubsystemSDL(std::ostream& stream, const std::string& window_title, bool shading_pipeline) throw (SubsystemException);
    virtual ~SubsystemSDL();

    virtual void initialize(Resources& resources);
    virtual const char *get_subsystem_name() const;
    virtual bool is_dedicated_server() const;

    virtual int get_zoom_factor() const;
    virtual void toggle_fullscreen();
    virtual bool is_fullscreen() const;
    virtual void set_mouse_position(int x, int y);
    virtual void enable_text_input();
    virtual void disable_text_input();
    virtual bool has_scanlines();
    virtual void set_scanlines(bool state);
    virtual void set_scanlines_intensity(float v);
    virtual float get_scanlines_intensity();
    virtual void set_deadzone_horizontal(int v);
    virtual void set_deadzone_vertical(int v);

    virtual TileGraphic *create_tilegraphic(int width, int height);
    virtual Audio *create_audio();
    virtual Shader *create_shader(const std::string& filename, ZipReader *zip);

    virtual void begin_drawings();
    virtual void end_drawings();
    virtual void set_color(float r, float g, float b, float a);
    virtual void reset_color();
    virtual void draw_tile(Tile *tile, int x, int y);
    virtual void draw_tilegraphic(TileGraphic *tilegraphic, int x, int y);
    virtual void draw_tilegraphic(TileGraphic *tilegraphic, int index, int x, int y);
    virtual void draw_box(int x, int y, int width, int height);
    virtual void draw_text(Font *font, int x, int y, const std::string& text);
    virtual int draw_char(Font *font, int x, int y, unsigned char c);
    virtual void draw_icon(Icon *icon, int x, int y);

    virtual int play_sound(Sound *sound, int loops);
    virtual void play_system_sound(Sound *sound);
    virtual int play_controlled_sound(Sound *sound, int loops);
    virtual bool is_sound_playing(Sound *sound);

    virtual bool play_music(Music *music);
    virtual void stop_music();
    virtual void start_music_player(Resources& resources, TextMessageSystem& tms, const char *directory = 0);
    virtual void skip_music_player_song();
    virtual void stop_music_player();

    virtual void set_music_volume(int v);
    virtual int get_music_volume();
    virtual void set_sound_volume(int v);

    virtual bool get_input(InputData& input);
    virtual void clear_input_buffer();
    virtual const char *get_keycode_name(int keycode);
    virtual const char *get_controller_name();
    virtual int rescan_joysticks();

    virtual int get_arena_width();
    virtual int get_arena_height();
    virtual int get_view_width();
    virtual int get_view_height();

private:
    typedef std::vector<SDL_Joystick *> Joysticks;
    enum WindowMode {
        WindowModeWindowed,
        WindowModeFullscreen
    };

    typedef void (SubsystemSDL::*QuadDrawer)(int x, int y, int w, int h, bool no_tex);

    SDL_Window *window;
    int joyaxis;
    bool fullscreen;
    bool draw_scanlines;
    float scanlines_intensity;
    int deadzone_horizontal;
    int deadzone_vertical;
    GLuint selected_tex;
    int music_volume;
    GLuint vao;
    GLuint vbo;
    Shader *base_shader;
    GLuint blank_tex;
    bool shading_pipeline;
    QuadDrawer draw_quad;

    SDL_GLContext glcontext;
    SDL_Event event;

    Joysticks joysticks;

    int native_width;
    int native_height;
    int current_zoom;
    int current_width;
    int current_height;
    int gl_width;
    int gl_height;
    int box_width;
    int box_height;
    float box_width_factor;
    float box_height_factor;
    GLfloat vertices[6][4];
    int fullscreen_zoom;
    int x_offset;
    int y_offset;
    WindowMode window_mode;
    Shader::ID shader_loc_color;
    Shader::ID shader_loc_projection_matrix;
    Shader::ID shader_loc_offset;

    void init_gl(int width, int height);
    void close_joysticks();
    int grab_joysticks();
    void calc_offsets();
    void draw_boxes();

    void set_window_icon(SDL_Window *window);
    void setup_projection();
    void draw_immediate(int x, int y, int width, int height, bool no_tex);
    void draw_vbo(int x, int y, int width, int height, bool no_tex);
};

#endif
