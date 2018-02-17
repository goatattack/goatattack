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

#ifndef DEDICATED_SERVER

#include "SubsystemSDL.hpp"
#include "TileGraphicGL.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "AudioSDL.hpp"
#include "Font.hpp"
#include "Icon.hpp"
#include "ShaderGL.hpp"
#include "ShaderNull.hpp"
#include "Resources.hpp"
#include "AutoPtr.hpp"

#include <cstring>
#include <algorithm>
#include <clocale>
#ifdef __unix__
#include <SDL2/SDL_opengl.h>
#ifdef __APPLE__
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#elif _WIN32
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#endif

namespace {

    const int ViewWidth = 640;
    const int ViewHeight = 340;
    const int WindowedZoomFactor = 2;
    const int DefaultOpenGLMajor = 3;
    const int DefaultOpenGLMinor = 1;

    typedef std::vector<Sound *> PlayingSounds;

    PlayingSounds playing_sounds;

    void add_playing_sound(Sound *sound) {
        if (std::find(playing_sounds.begin(), playing_sounds.end(), sound) == playing_sounds.end()) {
            playing_sounds.push_back(sound);
        }
    }

    void channel_done(int c) {
        for (PlayingSounds::iterator it = playing_sounds.begin();
            it != playing_sounds.end(); it++)
        {
            Sound *sound = *it;
            if (sound->get_playing_channel() == c) {
                playing_sounds.erase(it);
                break;
            }
        }
    }

    typedef std::vector<Music *> MusicPlayerMusics;

    Music *music_player_current_music = 0;
    int music_player_current_index = -1;
    TextMessageSystem *music_player_tms = 0;
    I18N *glbi18n = 0;
    MusicPlayerMusics music_player_musics;

    struct ExternalMusic {
        ExternalMusic(const std::string& filename, const std::string& shortname)
            : filename(filename), shortname(shortname) { }

        std::string filename;
        std::string shortname;
    };

    typedef std::vector<ExternalMusic> MusicPlayerExternalMusics;
    bool music_player_external = false;
    MusicPlayerExternalMusics music_player_external_musics;
    Mix_Music *music_player_external_music_handle = 0;
    int music_player_current_external_index = -1;
    time_t music_player_external_last_played = 0;
    std::string music_player_external_last_song;

    void play_next_song(bool print) {
        if (music_player_tms) {
            if (music_player_external) {
                if (music_player_external_music_handle) {
                    Mix_FreeMusic(music_player_external_music_handle);
                    music_player_external_music_handle = 0;
                }
                if (time(0) - music_player_external_last_played < 2) {
                    music_player_tms->add_text_msg((*glbi18n)(I18N_MUSIC_TOO_FAST, music_player_external_last_song));
                } else {
                    bool ok = false;
                    int count = 0;
                    int sz = static_cast<int>(music_player_external_musics.size());
                    while (count < sz) {
                        music_player_current_external_index++;
                        if (music_player_current_external_index >= sz) {
                            music_player_current_external_index = 0;
                        }
                        const ExternalMusic& em = music_player_external_musics[music_player_current_external_index];
                        music_player_external_music_handle = Mix_LoadMUS(em.filename.c_str());
                        if (music_player_external_music_handle) {
                            if (Mix_PlayMusic(music_player_external_music_handle, 0)) {
                                Mix_FreeMusic(music_player_external_music_handle);
                                music_player_external_music_handle = 0;
                            } else {
                                music_player_external_last_played = time(0);
                                music_player_external_last_song = em.shortname;
                                if (Mix_VolumeMusic(-1) || print) {
                                    music_player_tms->add_text_msg((*glbi18n)(I18N_MUSIC_INFO, em.shortname));
                                }
                                ok = true;
                                break;
                            }
                        }
                        /* try next one */
                        count++;
                    }
                    if (!ok) {
                        music_player_tms->add_text_msg((*glbi18n)(I18N_NO_MUSIC_FOUND));
                    }
                }
            } else {
                Music *music = 0;
                int sz = static_cast<int>(music_player_musics.size());
                if (sz) {
                    music_player_current_index++;
                    if (music_player_current_index >= sz) {
                        music_player_current_index = 0;
                    }
                    music = music_player_musics[music_player_current_index];
                    if (music != music_player_current_music) {
                        music_player_current_music = music;
                        if (Mix_VolumeMusic(-1) || print) {
                            music_player_tms->add_text_msg((*glbi18n)(I18N_MUSIC_INFO, music->get_description()));
                        }
                    }
                }
                if (music_player_current_music) {
                    const AudioSDL *audio = static_cast<const AudioSDL *>(music_player_current_music->get_audio());
                    Mix_PlayMusic(audio->get_music(), 0);
                }
            }
        }
    }

    void music_finished() {
        if (music_player_external) {
            play_next_song(false);
        } else {
            if (music_player_current_music) {
                play_next_song(false);
            }
        }
    }

    const int VertexCount = 2;
    const int TexUVCount = 2;
    const int Stride = (VertexCount + TexUVCount);
    const size_t StrideSize = Stride * sizeof(float);
    const void *VertexOffset = reinterpret_cast<void *>((0) * sizeof(float));
    const void *TexUVOffset = reinterpret_cast<void *>((0 + VertexCount) * sizeof(float));

#ifdef _WIN32
    const bool EnableShadingPipeline = false;
#else
    const bool EnableShadingPipeline = true;
#endif

}

SubsystemSDL::SubsystemSDL(std::ostream& stream, I18N& i18n, const std::string& window_title, bool shading_pipeline)
    : Subsystem(stream, i18n, window_title), window(0), joyaxis(0), fullscreen(false),
      draw_scanlines(false), scanlines_intensity(0.5f),
      deadzone_horizontal(3200), deadzone_vertical(3200), selected_tex(0),
      music_volume(100), vao(0), vbo(0), base_shader(0), blank_tex(0),
      shading_pipeline(shading_pipeline && EnableShadingPipeline),
      draw_quad(this->shading_pipeline ? &SubsystemSDL::draw_vbo : &SubsystemSDL::draw_immediate)
{
    stream << i18n(I18N_SSSDL_START) << std::endl;

    /* set locale to system default for proper mbstowcs() */
#ifndef _WIN32
    setlocale(LC_ALL, "");
#endif

    /* init SDL */
    stream << i18n(I18N_SSSDL_INIT) << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER)) {
        throw SubsystemException(SDL_GetError());
    }
    if (shading_pipeline) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, DefaultOpenGLMajor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, DefaultOpenGLMinor);
    }

    /* get native desktop size */
    SDL_DisplayMode display;
    if (SDL_GetCurrentDisplayMode(0, &display) < 0) {
        throw SubsystemException(i18n(I18N_SSSDL_SIZE_QUERY, std::string(SDL_GetError())));
    }
    native_width = display.w;
    native_height = display.h;
    int mw = native_width / ViewWidth;
    int mh = native_height / ViewHeight;
    fullscreen_zoom = (mh > mw ? mw : mh);

    x_offset = 0;
    y_offset = 0;

    /* setup screen rect */
    window_mode = WindowModeWindowed;
    current_zoom = WindowedZoomFactor;
    current_width = ViewWidth * current_zoom;
    current_height = ViewHeight * current_zoom;
    gl_width = current_width;
    gl_height = current_height;
    box_width = gl_width / current_zoom;
    box_height = gl_height / current_zoom;
    box_width_factor = 2.0f / static_cast<float>(box_width);
    box_height_factor = 2.0f / static_cast<float>(box_height);

    /* create window */
    stream << i18n(I18N_SSSDL_WINDOW) << std::endl;
    window = SDL_CreateWindow(
        window_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        current_width,
        current_height,
        SDL_WINDOW_OPENGL);
    if (!window) {
        throw SubsystemException(i18n(I18N_SSSDL_WINDOW_FAILED, std::string(SDL_GetError())));
    }
    SDL_ShowCursor(SDL_DISABLE);

    /* load window icon */
    set_window_icon(window);

    /* grab all joysticks */
    grab_joysticks();

    /* initialize OpenGL */
    stream << i18n(I18N_SSSDL_OPENGL) << std::endl;
    glcontext = SDL_GL_CreateContext(window);
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        SDL_GL_SetSwapInterval(1);
    }
    init_gl(gl_width, gl_height);

    /* init audio */
    if (!(Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) & MIX_INIT_OGG)) {
        throw SubsystemException(i18n(I18N_SSSDL_MIXER_FAILED, std::string(Mix_GetError())));
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        throw SubsystemException(i18n(I18N_SSSDL_AUDIO_FAILED, std::string(Mix_GetError())));
    }
    Mix_ChannelFinished(channel_done);
    Mix_AllocateChannels(128);
    Mix_ReserveChannels(1);

    Mix_HookMusicFinished(music_finished);
}

SubsystemSDL::~SubsystemSDL() {
    stream << i18n(I18N_SSSDL_UNINIT) << std::endl;

#ifdef __APPLE__
    if (is_fullscreen()) {
        toggle_fullscreen();
    }
#endif

#ifndef _WIN32
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
#endif

    close_joysticks();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void SubsystemSDL::initialize(Resources& resources) {
#ifndef _WIN32
    if (shading_pipeline) {
        stream << i18n(I18N_SSSDL_SHADING_PIPELINE) << std::endl;
        /* setup dynamic vao and vbo */
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 6 * StrideSize, 0, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, VertexCount, GL_FLOAT, GL_FALSE, StrideSize, VertexOffset);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, TexUVCount, GL_FLOAT, GL_FALSE, StrideSize, TexUVOffset);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        vertices[0][2] = 0.0f; vertices[0][3] = 0.0f;
        vertices[1][2] = 0.0f; vertices[1][3] = 1.0f;
        vertices[2][2] = 1.0f; vertices[2][3] = 1.0f;
        vertices[3][2] = 0.0f; vertices[3][3] = 0.0f;
        vertices[4][2] = 1.0f; vertices[4][3] = 1.0f;
        vertices[5][2] = 1.0f; vertices[5][3] = 0.0f;

        /* setup shader */
        base_shader = resources.get_shader("base");
        base_shader->activate();
        shader_loc_projection_matrix = base_shader->get_location("projection_matrix");
        shader_loc_color = base_shader->get_location("color");
        shader_loc_offset = base_shader->get_location("offset");
        base_shader->set_value(base_shader->get_location("tex"), 0);
        blank_tex = static_cast<TileGraphicGL *>(resources.get_icon("blank")->get_tile()->get_tilegraphic())->get_texture();
        setup_projection();
    }
#endif

    reset_color();
}

const char *SubsystemSDL::get_subsystem_name() const {
    return "SubsystemSDL";
}

bool SubsystemSDL::is_dedicated_server() const {
    return false;
}

int SubsystemSDL::get_zoom_factor() const {
    return current_zoom;
}

void SubsystemSDL::toggle_fullscreen() {
    int flag = 0;

    stream << i18n(I18N_SSSDL_FULLSCREEN) << std::endl;
    fullscreen = !fullscreen;

    if (!fullscreen) {
        SDL_SetWindowFullscreen(window, 0);
        current_zoom = WindowedZoomFactor;
        x_offset = 0;
        y_offset = 0;
    } else {
#ifdef __APPLE__
        flag = SDL_WINDOW_FULLSCREEN;
#else
        flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
        current_zoom = fullscreen_zoom;
        calc_offsets();
    }

    current_width = ViewWidth * current_zoom;
    current_height = ViewHeight * current_zoom;

#ifdef __APPLE__
    if (fullscreen) {
        SDL_SetWindowSize(window, native_width, native_height);
    } else {
        SDL_SetWindowSize(window, current_width, current_height);
    }
#else
    SDL_SetWindowSize(window, current_width, current_height);
#endif

    SDL_SetWindowFullscreen(window, flag);

    if (fullscreen) {
        gl_width = native_width;
        gl_height = native_height;
    } else {
        gl_width = current_width;
        gl_height = current_height;
    }
    init_gl(gl_width, gl_height);
    box_width = gl_width / current_zoom;
    box_height = gl_height / current_zoom;
    box_width_factor = 2.0f / static_cast<float>(box_width);
    box_height_factor = 2.0f / static_cast<float>(box_height);
    setup_projection();

    if (!fullscreen) {
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    SDL_SetWindowGrab(window, (fullscreen ? SDL_TRUE : SDL_FALSE));
}

bool SubsystemSDL::is_fullscreen() const {
    return fullscreen;
}

void SubsystemSDL::set_mouse_position(int x, int y) {
    SDL_WarpMouseInWindow(window, x * current_zoom, y * current_zoom);
}

void SubsystemSDL::enable_text_input() {
    SDL_StartTextInput();
}

void SubsystemSDL::disable_text_input() {
    SDL_StopTextInput();
}

bool SubsystemSDL::has_scanlines() {
    return draw_scanlines;
}

void SubsystemSDL::set_scanlines(bool state) {
    draw_scanlines = state;
}

void SubsystemSDL::set_scanlines_intensity(float v) {
    scanlines_intensity = v;
}

float SubsystemSDL::get_scanlines_intensity() {
    return scanlines_intensity;
}

void SubsystemSDL::set_deadzone_horizontal(int v) {
    deadzone_horizontal = v;
}

void SubsystemSDL::set_deadzone_vertical(int v) {
    deadzone_vertical = v;
}

TileGraphic *SubsystemSDL::create_tilegraphic(int width, int height) {
    return new TileGraphicGL(width, height, keep_pictures);
}

Audio *SubsystemSDL::create_audio() {
    return new AudioSDL;
}

Shader *SubsystemSDL::create_shader(const std::string& filename, ZipReader *zip) {
#ifndef _WIN32
    if (shading_pipeline) {
        return new ShaderGL(*this, filename, zip);
    }
#endif

    return new ShaderNull(filename, zip);
}

void SubsystemSDL::begin_drawings() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SubsystemSDL::end_drawings() {
    if (scanlines && draw_scanlines) {
        set_color(1.0f, 1.0f, 1.0f, scanlines_intensity);
        int x, y;
        TileGraphicGL *tg = static_cast<TileGraphicGL *>(scanlines->get_tile()->get_tilegraphic());
        int w = tg->get_width() / 2;
        int h = tg->get_height() / 2;
        glBindTexture(GL_TEXTURE_2D, tg->get_texture(tg->get_current_index()));
        y = 0;
        while (y < ViewHeight) {
            x = 0;
            while (x < ViewWidth) {
                (this->*draw_quad)(x, y, w, h, false);
                x += w;
            }
            y += h;
        }
    }

    reset_color();
    draw_boxes();
    SDL_GL_SwapWindow(window);
}

void SubsystemSDL::set_color(float r, float g, float b, float a) {
    if (shading_pipeline) {
        base_shader->set_value(shader_loc_color, r, g, b, a);
    } else {
        glColor4f(r, g, b, a);
    }
}

void SubsystemSDL::reset_color() {
    set_color(1.0f, 1.0f, 1.0f, 1.0f);
}

void SubsystemSDL::draw_tile(Tile *tile, int x, int y) {
    draw_tilegraphic(tile->get_tilegraphic(), x, y);
}

void SubsystemSDL::draw_tilegraphic(TileGraphic *tilegraphic, int x, int y) {
    TileGraphicGL *tg = static_cast<TileGraphicGL *>(tilegraphic);
    draw_tilegraphic(tilegraphic, tg->get_current_index(), x, y);
}

void SubsystemSDL::draw_tilegraphic(TileGraphic *tilegraphic, int index, int x, int y) {
    TileGraphicGL *tg = static_cast<TileGraphicGL *>(tilegraphic);
    const int& width = tg->get_width();
    const int& height = tg->get_height();

    /* switch texture, if needed */
    GLuint tex = tg->get_texture(index);
    if (tex != selected_tex) {
        glBindTexture(GL_TEXTURE_2D, tex);
        selected_tex = tex;
    }
    (this->*draw_quad)(x, y, width, height, false);
}

void SubsystemSDL::draw_box(int x, int y, int width, int height) {
    (this->*draw_quad)(x, y, width, height, true);
}

int SubsystemSDL::draw_text(Font *font, int x, int y, const std::string& text) {
    size_t sz = text.length();
    const Font::Character *prev = 0;

    if (sz) {
        int y_offset = font->get_y_offset();
        const char *p = text.c_str();
        while (*p) {
            const Font::Character *chr = font->get_character(p);
            draw_tilegraphic(chr->tile->get_tilegraphic(), x, y + y_offset + chr->y_offset);
            x += chr->advance + font->get_x_kerning(prev, chr);
            p += chr->distance;
            prev = chr;
        }
    }

    return x;
}

int SubsystemSDL::draw_clipped_text(Font *font, int x, int y, int width, const std::string& text) {
    size_t sz = text.length();
    const Font::Character *prev = 0;

    if (sz) {
        font->clip_on(x, y, width);
        int totw = 0;
        const char *p = text.c_str();
        int font_y_offset = font->get_y_offset();
        while (*p) {
            const Font::Character *chr = font->get_character(p);
            int advance = chr->advance + font->get_x_kerning(prev, chr);
            draw_tilegraphic(chr->tile->get_tilegraphic(), x, y + font_y_offset + chr->y_offset);
            x += advance;
            totw += advance;
            if (totw > width) {
                break;
            }
            p += chr->distance;
            prev = chr;
        }
        font->clip_off();
    }

    return x;
}

int SubsystemSDL::draw_char(Font *font, int x, int y, const char *s) {
    int y_offset = font->get_y_offset();
    const Font::Character *chr = font->get_character(s);
    draw_tilegraphic(chr->tile->get_tilegraphic(), x, y + y_offset + chr->y_offset);
    x += chr->advance;

    return x;
}

void SubsystemSDL::draw_icon(Icon *icon, int x, int y) {
    draw_tile(icon->get_tile(), x, y);
}


void SubsystemSDL::enable_cliprect(int x, int y, int width, int height) {
    glScissor(
        ((x + x_offset) * current_zoom),
        (ViewHeight - (y - y_offset)) * current_zoom,
        width * current_zoom,
        height * current_zoom
    );
    glEnable(GL_SCISSOR_TEST);
}

void SubsystemSDL::disable_cliprect() {
    glDisable(GL_SCISSOR_TEST);
}

int SubsystemSDL::play_sound(Sound *sound, int loops) {
    int channel = -1;
    if (sound) {
        const AudioSDL *audio = static_cast<const AudioSDL *>(sound->get_audio());
        int c = Mix_GroupAvailable(-1);
        if (c < 0) {
            int c = Mix_GroupOldest(-1);
            if (c >= 0) {
                Mix_HaltChannel(c);
                channel_done(c);
            }
        }
        channel = Mix_PlayChannel(-1, audio->get_chunk(), loops);
    }

    return channel;
}

void SubsystemSDL::play_system_sound(Sound *sound) {
    if (sound) {
        Mix_HaltChannel(0);
        const AudioSDL *audio = static_cast<const AudioSDL *>(sound->get_audio());
        Mix_PlayChannel(0, audio->get_chunk(), 0);
    }
}

int SubsystemSDL::play_controlled_sound(Sound *sound, int loops) {
    if (sound) {
        const AudioSDL *audio = static_cast<const AudioSDL *>(sound->get_audio());
        int c = Mix_PlayChannel(-1, audio->get_chunk(), loops);
        if (c != -1) {
            sound->set_playing_channel(c);
            add_playing_sound(sound);
        }
        return c;
    } else {
        return -1;
    }
}

bool SubsystemSDL::is_sound_playing(Sound *sound) {
    return (std::find(playing_sounds.begin(), playing_sounds.end(), sound) != playing_sounds.end());
}

int SubsystemSDL::stop_sound(int channel) {
    if (channel > -1) {
        Mix_HaltChannel(channel);
    }

    return -1;
}

bool SubsystemSDL::play_music(Music *music) {
    music_player_current_music = 0;
    if (music) {
        const AudioSDL *audio = static_cast<const AudioSDL *>(music->get_audio());
        Mix_PlayMusic(audio->get_music(), -1);
        return true;
    }

    return false;
}

void SubsystemSDL::stop_music() {
    music_player_tms = 0;
    music_player_current_music = 0;
    music_player_musics.clear();
    Mix_HaltMusic();

    /* stop external music */
    if (music_player_external_music_handle) {
        Mix_FreeMusic(music_player_external_music_handle);
        music_player_external_music_handle = 0;
    }
}

void SubsystemSDL::start_music_player(Resources& resources, TextMessageSystem& tms, const char *directory) {
    music_player_tms = &tms;
    glbi18n = &i18n;
    bool take_internal_music = true;

    /* check if external music files exist */
    if (directory && strlen(directory)) {
        music_player_external = true;
        const char *entry = 0;
        const char *suffixes[] = { ".ogg", ".mp3", ".flac", 0 };
        try {
            const char **suffix = suffixes;
            while (*suffix) {
                Directory dir(directory, *suffix, 0);
                while ((entry = dir.get_entry())) {
                    music_player_external_musics.push_back(ExternalMusic(std::string(directory) + dir_separator + entry + *suffix, std::string(entry) + *suffix));
                }
                suffix++;
            }
        } catch (const Exception& e) {
            music_player_external_musics.clear();
            tms.add_text_msg(std::string("ERROR: Cannot read files: ") + e.what());
        }
        if (!music_player_external_musics.size()) {
            tms.add_text_msg("ERROR: No .ogg or .mp3 files found, playing from internal pak.");
        } else {
            std::random_shuffle(music_player_external_musics.begin(), music_player_external_musics.end());
            music_player_current_external_index = -1;
            music_player_external_last_played = 0;
            take_internal_music = false;
        }
    }

    /* take internal music list? */
    if (take_internal_music) {
        music_player_external = false;
        music_player_musics.clear();
        Resources::ResourceObjects& musics = resources.get_musics();
        for (Resources::ResourceObjects::iterator it = musics.begin(); it != musics.end(); it++) {
            Resources::ResourceObject& obj = *it;
            Music *music = static_cast<Music *>(obj.object);
            if (!music->get_do_not_play_in_music_player()) {
                music_player_musics.push_back(music);
            }
        }
        std::random_shuffle(music_player_musics.begin(), music_player_musics.end());
    }

    play_next_song(false);
}

void SubsystemSDL::skip_music_player_song() {
    music_player_external_last_played = 0;
    play_next_song(true);
}

void SubsystemSDL::stop_music_player() {
    stop_music();
}

void SubsystemSDL::set_music_volume(int v) {
    music_volume = v;
    Mix_VolumeMusic(v);
}

int SubsystemSDL::get_music_volume() {
    return music_volume;
}

void SubsystemSDL::set_sound_volume(int v) {
    Mix_Volume(-1, v);
}

bool SubsystemSDL::get_input(InputData& input) {
    if (SDL_PollEvent(&event)) {
        memset(&input, 0, sizeof(input));
        switch (event.type) {
            case SDL_QUIT:
                input.data_type = InputData::InputDataTypeQuit;
                break;

            case SDL_MOUSEMOTION:
                input.data_type = InputData::InputDataTypeMouseMove;
                input.param1 = event.motion.x;
                input.param2 = event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                input.param1 = event.motion.x;
                input.param2 = event.motion.y;
                switch (event.button.button) {
                    case SDL_BUTTON_MIDDLE:
                        input.data_type = InputData::InputDataTypeMouseMiddleDown;
                        break;

                    case SDL_BUTTON_RIGHT:
                        input.data_type = InputData::InputDataTypeMouseRightDown;
                        break;

                    default:
                        input.data_type = InputData::InputDataTypeMouseLeftDown;
                        break;
                }

                break;

            case SDL_MOUSEBUTTONUP:
                input.param1 = event.motion.x;
                input.param2 = event.motion.y;
                switch (event.button.button) {
                    case SDL_BUTTON_MIDDLE:
                        input.data_type = InputData::InputDataTypeMouseMiddleUp;
                        break;

                    case SDL_BUTTON_RIGHT:
                        input.data_type = InputData::InputDataTypeMouseRightUp;
                        break;

                    default:
                        input.data_type = InputData::InputDataTypeMouseLeftUp;
                        break;
                }
                break;

            case SDL_MOUSEWHEEL:
                input.param1 = event.motion.x;
                input.param2 = event.motion.y;
                input.data_type = InputData::InputDataTypeMouseWheel;
                break;

            case SDL_WINDOWEVENT:
            {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_LEAVE:
                        input.data_type = InputData::InputDataMouseLeftWindow;
                        break;

                    case SDL_WINDOWEVENT_ENTER:
                        input.data_type = InputData::InputDataMouseEnteredWindow;
                        break;

                    default:
                        input.data_type = InputData::InputDataTypeNothing;
                        break;
                }
                break;
            }

            case SDL_TEXTINPUT:
            {
                input.data_type = InputData::InputDataTypeText;
                input.text = reinterpret_cast<const unsigned char *>(event.text.text);
                break;
            }

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                input.key_repeat = event.key.repeat;
                input.keycode = event.key.keysym.sym;
                input.data_type = (event.type == SDL_KEYDOWN ?
                    InputData::InputDataTypeKeyDown : InputData::InputDataTypeKeyUp);
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        input.key_type = InputData::InputKeyTypeLeft;
                        break;

                    case SDLK_RIGHT:
                        input.key_type = InputData::InputKeyTypeRight;
                        break;

                    case SDLK_UP:
                        input.key_type = InputData::InputKeyTypeUp;
                        break;

                    case SDLK_DOWN:
                        input.key_type = InputData::InputKeyTypeDown;
                        break;

                    case SDLK_BACKSPACE:
                        input.key_type = InputData::InputKeyTypeBackspace;
                        break;

                    case SDLK_DELETE:
                        input.key_type = InputData::InputKeyTypeDelete;
                        break;

                    case SDLK_HOME:
                        input.key_type = InputData::InputKeyTypeHome;
                        break;

                    case SDLK_END:
                        input.key_type = InputData::InputKeyTypeEnd;
                        break;

                    case SDLK_PAGEUP:
                        input.key_type = InputData::InputKeyTypePageUp;
                        break;

                    case SDLK_PAGEDOWN:
                        input.key_type = InputData::InputKeyTypePageDown;
                        break;

                    case SDLK_ESCAPE:
                        input.key_type = InputData::InputKeyTypeEscape;
                        break;

                    case SDLK_SPACE:
                        input.key_type = InputData::InputKeyTypeButtonA;
                        break;

                    case SDLK_a:
                        input.key_type = InputData::InputKeyTypeButtonB;
                        break;

                    case SDLK_s:
                        input.key_type = InputData::InputKeyTypeButtonX;
                        break;

                    case SDLK_d:
                        input.key_type = InputData::InputKeyTypeButtonY;
                        break;

                    case SDLK_t:
                        input.key_type = InputData::InputKeyTypeChat;
                        break;

                    case SDLK_TAB:
                        input.key_type = InputData::InputKeyTypeStatistics;
                        break;

                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        input.key_type = InputData::InputKeyTypeShift;
                        break;

                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        input.key_type = InputData::InputKeyTypeControl;
                        break;
                }
                break;

            case SDL_JOYAXISMOTION:
            {
                input.param1 = joyaxis;
                if (event.jaxis.axis == 0) {
                    /* horizontal */
                    if (event.jaxis.value < -deadzone_horizontal) {
                        input.param1 |= InputData::InputJoyDirectionLeft;
                        input.param1 &= ~InputData::InputJoyDirectionRight;
                    } else if (event.jaxis.value > deadzone_horizontal) {
                        input.param1 |= InputData::InputJoyDirectionRight;
                        input.param1 &= ~InputData::InputJoyDirectionLeft;
                    } else {
                        input.param1 &= ~(InputData::InputJoyDirectionLeft | InputData::InputJoyDirectionRight);
                    }
                }
                if (event.jaxis.axis == 1) {
                    /* vertical */
                    if (event.jaxis.value < -deadzone_vertical) {
                        input.param1 |= InputData::InputJoyDirectionUp;
                        input.param1 &= ~InputData::InputJoyDirectionDown;
                    } else if (event.jaxis.value > deadzone_vertical) {
                        input.param1 |= InputData::InputJoyDirectionDown;
                        input.param1 &= ~InputData::InputJoyDirectionUp;
                    } else {
                        input.param1 &= ~(InputData::InputJoyDirectionUp | InputData::InputJoyDirectionDown);
                    }
                }
                joyaxis = input.param1;
                input.data_type = InputData::InputDataTypeJoyMotion;
                break;
            }

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                input.data_type = (event.type == SDL_JOYBUTTONDOWN ?
                    InputData::InputDataTypeJoyButtonDown : InputData::InputDataTypeJoyButtonUp);
                input.param1 = event.jbutton.button;
                break;
        }

        return true;
    }
    return false;
}

void SubsystemSDL::clear_input_buffer() {
    while (SDL_PollEvent(&event));
}

const char *SubsystemSDL::get_keycode_name(int keycode) {
    return SDL_GetKeyName(keycode);
}

int SubsystemSDL::rescan_joysticks() {
    return grab_joysticks();
}

const char *SubsystemSDL::get_controller_name() {
    if (joysticks.size()) {
        return SDL_JoystickName(joysticks[0]);
    }

    return "";
}

int SubsystemSDL::get_arena_width() {
    return current_width;
}

int SubsystemSDL::get_arena_height() {
    return current_height;
}

int SubsystemSDL::get_view_width() {
    return ViewWidth;
}

int SubsystemSDL::get_view_height() {
    return ViewHeight;
}

void SubsystemSDL::init_gl(int width, int height) {
    stream << i18n(I18N_SSSDL_OPENGL_INIT) << std::endl;
    /* init gl scene */
    glViewport(0, 0, width, height);

    /* immediate mode settings */
    if (!shading_pipeline) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, width, height, 0, 0, 1);
        glEnable(GL_TEXTURE_2D);
    }

    /* no depth testing */
    glDisable(GL_DEPTH_TEST);

    /* enable alpha channel */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* backface culling */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /* make screen black initially */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

void SubsystemSDL::close_joysticks() {
    for (Joysticks::iterator it = joysticks.begin(); it != joysticks.end(); it++) {
        SDL_Joystick *joystick = *it;
        SDL_JoystickClose(joystick);
    }
    joysticks.clear();
}

int SubsystemSDL::grab_joysticks() {
    close_joysticks();

    int sz = SDL_NumJoysticks();
    for (int i = 0; i < sz; i++) {
        joysticks.push_back(SDL_JoystickOpen(i));
    }

    return joysticks.size();
}

void SubsystemSDL::calc_offsets() {
    x_offset = (native_width - (ViewWidth * current_zoom)) / 2 / current_zoom;
    y_offset = (native_height - (ViewHeight * current_zoom)) / 2 / current_zoom;
}

void SubsystemSDL::draw_boxes() {
    if (y_offset) {
        set_color(0.0f, 0.0f, 0.0f, 1.0f);
        draw_box(-x_offset, -y_offset, box_width, y_offset);
        draw_box(-x_offset, box_height - 2 * y_offset, box_width, y_offset);
    }
    if (x_offset) {
        set_color(0.0f, 0.0f, 0.0f, 1.0f);
        draw_box(-x_offset, 0, x_offset, box_height - 2 * y_offset);
        draw_box(box_width - 2 * x_offset, 0, x_offset, box_height - 2 * y_offset);
    }
    reset_color();
}

void SubsystemSDL::set_window_icon(SDL_Window *window) {
#ifdef _WIN32
    const unsigned int mask_r = 0x00ff0000;
    const unsigned int mask_g = 0x0000ff00;
    const unsigned int mask_b = 0x000000ff;
    const unsigned int mask_a = 0xff000000;
    const int res_id = 101;
    const int size = 32;
    const int bpp = 32;

    HICON icon = static_cast<HICON>(LoadImage(GetModuleHandle(0),
        MAKEINTRESOURCE(res_id), IMAGE_ICON, size, size, LR_SHARED));

    if (icon) {
        ICONINFO ici;

        if (GetIconInfo(icon, &ici)) {
            HDC dc = CreateCompatibleDC(0);

            if (dc) {
                SDL_Surface *surface = SDL_CreateRGBSurface(0, size, size, bpp, mask_r, mask_g, mask_b, mask_a);

                if (surface) {
                    BITMAPINFO bmi;
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = size;
                    bmi.bmiHeader.biHeight = -size;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = bpp;
                    bmi.bmiHeader.biCompression = BI_RGB;
                    bmi.bmiHeader.biSizeImage = 0;

                    SelectObject(dc, ici.hbmColor);
                    GetDIBits(dc, ici.hbmColor, 0, size, surface->pixels, &bmi, DIB_RGB_COLORS);
                    SDL_SetWindowIcon(window, surface);
                    SDL_FreeSurface(surface);
                }
                DeleteDC(dc);
            }
            DeleteObject(ici.hbmColor);
            DeleteObject(ici.hbmMask);
        }
        DestroyIcon(icon);
    }
#endif
}

void SubsystemSDL::setup_projection() {
    if (shading_pipeline) {
        Matrix4x4 ortho;
        ortho.set_ortho(0.0f, static_cast<float>(box_width), 0.0f, static_cast<float>(box_height), 0.0f, 1.0f);
        base_shader->set_value(shader_loc_projection_matrix, ortho);
        base_shader->set_value(shader_loc_offset, static_cast<float>(x_offset), static_cast<float>(-y_offset + box_height));
    }
}

void SubsystemSDL::draw_immediate(int x, int y, int width, int height, bool no_tex) {
    if (no_tex) {
        glDisable(GL_TEXTURE_2D);
    }
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(((x_offset + x) * current_zoom), ((y_offset + y) * current_zoom));
    glTexCoord2i(0, 1);
    glVertex2i(((x_offset + x) * current_zoom), ((y_offset + y + height) * current_zoom));
    glTexCoord2i(1, 1);
    glVertex2i(((x_offset + x + width) * current_zoom), ((y_offset + y + height) * current_zoom));
    glTexCoord2i(1, 0);
    glVertex2i(((x_offset + x + width) * current_zoom), ((y_offset + y) * current_zoom));
    glEnd();
    if (no_tex) {
        glEnable(GL_TEXTURE_2D);
    }
}

void SubsystemSDL::draw_vbo(int x, int y, int width, int height, bool no_tex) {
#ifndef _WIN32
    if (no_tex) {
        glBindTexture(GL_TEXTURE_2D, blank_tex);
        selected_tex = blank_tex;
    }

    float xpos = x;
    float ypos = -(y + height);
    float w = width;
    float h = height;

    vertices[0][0] = xpos;     vertices[0][1] = ypos + h;
    vertices[1][0] = xpos;     vertices[1][1] = ypos;
    vertices[2][0] = xpos + w; vertices[2][1] = ypos;

    vertices[3][0] = xpos;     vertices[3][1] = ypos + h;
    vertices[4][0] = xpos + w; vertices[4][1] = ypos;
    vertices[5][0] = xpos + w; vertices[5][1] = ypos + h;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
}

#endif
