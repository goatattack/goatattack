#include "SubsystemNull.hpp"
#include "TileGraphicNull.hpp"
#include "AudioNull.hpp"

SubsystemNull::SubsystemNull(std::ostream& stream, const std::string& window_title) throw (SubsystemException)
    : Subsystem(stream, window_title)
{
    stream << "starting SubsystemNull" << std::endl;
}

SubsystemNull::~SubsystemNull() {
    stream << "cleaning SubsystemNull" << std::endl;
}

const char *SubsystemNull::get_subsystem_name() const {
    return "SubsystemNull";
}

bool SubsystemNull::is_dedicated_server() const {
    return true;
}

int SubsystemNull::get_zoom_factor() const {
    return 1;
}

void SubsystemNull::toggle_fullscreen() { }

bool SubsystemNull::is_fullscreen() const {
    return false;
}

void SubsystemNull::set_mouse_position(int x, int y) { }

void SubsystemNull::enable_text_input() { }

void SubsystemNull::disable_text_input() { }

bool SubsystemNull::has_scanlines() {
    return false;
}

void SubsystemNull::set_scanlines(bool state) { }

float SubsystemNull::get_scanlines_intensity() {
    return 0;
}

void SubsystemNull::set_scanlines_intensity(float v) { }

void SubsystemNull::set_deadzone_horizontal(int v) { }

void SubsystemNull::set_deadzone_vertical(int v) { }

TileGraphic *SubsystemNull::create_tilegraphic(int width, int height) {
    return new TileGraphicNull(width, height, keep_pictures);
}

Audio *SubsystemNull::create_audio() {
    return new AudioNull;
}

void SubsystemNull::begin_drawings() { }

void SubsystemNull::end_drawings() { }

void SubsystemNull::set_color(float r, float g, float b, float a) { }

void SubsystemNull::reset_color() { }

void SubsystemNull::draw_tile(Tile *tile, int x, int y) { }

void SubsystemNull::draw_tilegraphic(TileGraphic *tilegraphic, int x, int y) { }

void SubsystemNull::draw_tilegraphic(TileGraphic *tilegraphic, int index, int x, int y) { }

void SubsystemNull::draw_box(int x, int y, int width, int height) { }

void SubsystemNull::draw_text(Font *font, int x, int y, const std::string& text) { }

int SubsystemNull::draw_char(Font *font, int x, int y, unsigned char c) {
    return x;
}

void SubsystemNull::draw_icon(Icon *icon, int x, int y) { }

int SubsystemNull::play_sound(Sound *sound, int loops) {
    return 0;
}

void SubsystemNull::play_system_sound(Sound *sound) { }

void SubsystemNull::stop_music() { }

int SubsystemNull::play_controlled_sound(Sound *sound, int loops) {
    return 0;
}

bool SubsystemNull::is_sound_playing(Sound *sound) {
    return false;
}

bool SubsystemNull::play_music(Music *music) {
    return false;
}

void SubsystemNull::start_music_player(Resources& resources, TextMessageSystem& tms) { }

void SubsystemNull::skip_music_player_song() { }

void SubsystemNull::stop_music_player() { }

bool SubsystemNull::get_input(InputData& input) {
    return false;
}

void SubsystemNull::set_music_volume(int v, bool in_game) { }

int SubsystemNull::get_music_volume() {
    return 0;
}

void SubsystemNull::set_relative_music_volume(int v) { }

void SubsystemNull::set_sound_volume(int v) { }

void SubsystemNull::clear_input_buffer() { }

const char *SubsystemNull::get_keycode_name(int keycode) {
    return "";
}

int SubsystemNull::rescan_joysticks() {
    return 0;
}

const char *SubsystemNull::get_controller_name() {
    return "";
}

int SubsystemNull::get_arena_width() {
    return 0;
}

int SubsystemNull::get_arena_height() {
    return 0;
}

int SubsystemNull::get_view_width() {
    return 0;
}

int SubsystemNull::get_view_height() {
    return 0;
}
