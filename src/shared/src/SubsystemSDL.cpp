#ifndef DEDICATED_SERVER

#include "SubsystemSDL.hpp"
#include "TileGraphicGL.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "AudioSDL.hpp"
#include "Font.hpp"
#include "Icon.hpp"
#include "Resources.hpp"

#include <cstring>
#include <algorithm>
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

static const int ViewWidth = 640;
static const int ViewHeight = 340;
static const int WindowedZoomFactor = 2;

typedef std::vector<Sound *> PlayingSounds;

static PlayingSounds playing_sounds;

static void add_playing_sound(Sound *sound) {
    if (std::find(playing_sounds.begin(), playing_sounds.end(), sound) == playing_sounds.end()) {
        playing_sounds.push_back(sound);
    }
}

static void channel_done(int c) {
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
MusicPlayerMusics music_player_musics;

static void play_next_song() {
    if (music_player_tms) {
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
                std::string msg("music: " + music->get_description() + " by " + music->get_author());
                music_player_tms->add_text_msg(msg);
            }
        }
        if (music_player_current_music) {
            const AudioSDL *audio = static_cast<const AudioSDL *>(music_player_current_music->get_audio());
            Mix_PlayMusic(audio->get_music(), 0);
        }
    }
}

static void music_finished() {
    if (music_player_current_music) {
        play_next_song();
    }
}

SubsystemSDL::SubsystemSDL(std::ostream& stream, const std::string& window_title) throw (SubsystemException)
    : Subsystem(stream, window_title), window(0), joyaxis(0), fullscreen(false),
      draw_scanlines(false), scanlines_intensity(0.5f),
      deadzone_horizontal(3200), deadzone_vertical(3200), selected_tex(0),
      music_volume(0), relative_music_volume(100)
{
    stream << "starting SubsystemSDL" << std::endl;

    /* init SDL */
    stream << "initializing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER)) {
        throw SubsystemException(SDL_GetError());
    }

    /* get native desktop size */
    SDL_DisplayMode display;
    if (SDL_GetCurrentDisplayMode(0, &display) < 0) {
        throw SubsystemException("Could not query main desktop size: " +
            std::string(SDL_GetError()));
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

    /* create window */
    stream << "creating SDL window" << std::endl;
    window = SDL_CreateWindow(
        window_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        current_width,
        current_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);
    if (!window) {
        throw SubsystemException("Could not create window: " +
            std::string(SDL_GetError()));
    }
    SDL_ShowCursor(SDL_DISABLE);

    /* load window icon */
    set_window_icon(window);

    /* grab all joysticks */
    grab_joysticks();

    /* initialize OpenGL */
    stream << "creating OpenGL context" << std::endl;
    glcontext = SDL_GL_CreateContext(window);
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        SDL_GL_SetSwapInterval(1);
    }
    init_gl(gl_width, gl_height);

    /* init audio */
    if (!(Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG)) {
        throw SubsystemException("Could not initialize mixer: " +
            std::string(Mix_GetError()));
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        throw SubsystemException("Could not open audio: " +
            std::string(Mix_GetError()));
    }
    Mix_ChannelFinished(channel_done);
    Mix_ReserveChannels(1);

    Mix_HookMusicFinished(music_finished);
}

SubsystemSDL::~SubsystemSDL() {
    stream << "cleaning SubsystemSDL" << std::endl;
#ifdef __APPLE__
    if (is_fullscreen()) {
        toggle_fullscreen();
    }
#endif
    close_joysticks();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
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

    stream << "toggling fullscreen" << std::endl;
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

    if (!fullscreen) {
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
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

void SubsystemSDL::begin_drawings() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SubsystemSDL::end_drawings() {
    if (scanlines && draw_scanlines) {
        set_color(1.0f, 1.0f, 1.0f, scanlines_intensity);
        float x, y;
        TileGraphicGL *tg = static_cast<TileGraphicGL *>(scanlines->get_tile()->get_tilegraphic());
        float zf = current_zoom / 2.0f;
        float w = tg->get_width() * zf;
        float h = tg->get_height() * zf;
        glBindTexture(GL_TEXTURE_2D, tg->get_texture(tg->get_current_index()));
        y = 0;
        while (y < gl_height) {
            x = 0;
            while (x < gl_width) {
                glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex2f(x, y);
                glTexCoord2i(0, 1);
                glVertex2f(x, y + h);
                glTexCoord2i(1, 1);
                glVertex2f(x + w, y + h);
                glTexCoord2i(1, 0);
                glVertex2f(x + w, y);
                glEnd();
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
    glColor4f(r, g, b, a);
}

void SubsystemSDL::reset_color() {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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

    /* draw quad */
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
}

void SubsystemSDL::draw_box(int x, int y, int width, int height) {
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2i(((x_offset + x) * current_zoom), ((y_offset + y) * current_zoom));
    glVertex2i(((x_offset + x) * current_zoom), ((y_offset + y + height) * current_zoom));
    glVertex2i(((x_offset + x + width) * current_zoom), ((y_offset + y + height) * current_zoom));
    glVertex2i(((x_offset + x + width) * current_zoom), ((y_offset + y) * current_zoom));
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void SubsystemSDL::draw_text(Font *font, int x, int y, const std::string& text) {
    size_t sz = text.length();

    for (size_t i = 0; i < sz; i++) {
        int c = text[i];
        if (c >= FontMin && c <= FontMax) {
            c -= FontMin;
            TileGraphic *tg = font->get_tile(c)->get_tilegraphic();
            draw_tilegraphic(tg, x, y);
            x += font->get_fw(c) + font->get_spacing();
        }
    }
}

int SubsystemSDL::draw_char(Font *font, int x, int y, unsigned char c) {
    if (c >= FontMin && c <= FontMax) {
        c -= FontMin;
        TileGraphic *tg = font->get_tile(c)->get_tilegraphic();
        draw_tilegraphic(tg, x, y);
        x += font->get_fw(c) + font->get_spacing();
    }

    return x;
}

void SubsystemSDL::draw_icon(Icon *icon, int x, int y) {
    draw_tile(icon->get_tile(), x, y);
}

int SubsystemSDL::play_sound(Sound *sound, int loops) {
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
        return Mix_PlayChannel(-1, audio->get_chunk(), loops);
    } else {
        return -1;
    }
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
}

void SubsystemSDL::start_music_player(Resources& resources, TextMessageSystem& tms) {
    music_player_tms = &tms;
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

    play_next_song();
}

void SubsystemSDL::skip_music_player_song() {
    play_next_song();
}

void SubsystemSDL::stop_music_player() {
    stop_music();
}

void SubsystemSDL::set_music_volume(int v, bool in_game) {
    music_volume = v;
    if (in_game) {
        set_relative_music_volume(relative_music_volume);
    } else {
        Mix_VolumeMusic(v);
    }
}

int SubsystemSDL::get_music_volume() {
    return music_volume;
}

void SubsystemSDL::set_relative_music_volume(int v) {
    relative_music_volume = v;
    float p = static_cast<float>(v);
    if (v < 0 || v > 100) {
        p = 100.0f;
    }

    int rv = (music_volume / 100.0f * p);
    Mix_VolumeMusic(rv);
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
    stream << "initializing OpenGL" << std::endl;
    /* init gl scene */
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 0, 1);

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* enable alpha channel */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* backface culling */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /* enable 2d textures */
    glEnable(GL_TEXTURE_2D);

    /* make screen black initially */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

#endif
