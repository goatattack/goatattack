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

#include "SubsystemNull.hpp"
#include "TileGraphicNull.hpp"
#include "AudioNull.hpp"
#include "ShaderNull.hpp"

SubsystemNull::SubsystemNull(std::ostream& stream, I18N& i18n, const std::string& window_title)
    : Subsystem(stream, i18n, window_title), i18n(i18n)
{
    stream << i18n(I18N_SSNULL_START) << std::endl;
}

SubsystemNull::~SubsystemNull() {
    stream << i18n(I18N_SSNULL_UNINIT) << std::endl;
}

void SubsystemNull::initialize(Resources& resources) { }

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

Shader *SubsystemNull::create_shader(const std::string& filename, ZipReader *zip) {
    return new ShaderNull(filename, zip);
}

void SubsystemNull::begin_drawings() { }

void SubsystemNull::end_drawings() { }

void SubsystemNull::set_color(float r, float g, float b, float a) { }

void SubsystemNull::reset_color() { }

void SubsystemNull::draw_tile(Tile *tile, int x, int y) { }

void SubsystemNull::draw_tilegraphic(TileGraphic *tilegraphic, int x, int y) { }

void SubsystemNull::draw_tilegraphic(TileGraphic *tilegraphic, int index, int x, int y) { }

void SubsystemNull::draw_box(int x, int y, int width, int height) { }

int SubsystemNull::draw_text(Font *font, int x, int y, const std::string& text) {
    return 0;
}

int SubsystemNull::draw_clipped_text(Font *font, int x, int y, int width, const std::string& text) {
    return 0;
}

int SubsystemNull::draw_char(Font *font, int x, int y, const char *s) {
    return x;
}

void SubsystemNull::draw_icon(Icon *icon, int x, int y) { }

void SubsystemNull::enable_cliprect(int x, int y, int width, int height) { }

void SubsystemNull::disable_cliprect() { }

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

int SubsystemNull::stop_sound(int channel) {
    return 0;
}

bool SubsystemNull::play_music(Music *music) {
    return false;
}

void SubsystemNull::start_music_player(Resources& resources, TextMessageSystem& tms, const char *directory) { }

void SubsystemNull::skip_music_player_song() { }

void SubsystemNull::stop_music_player() { }

bool SubsystemNull::get_input(InputData& input) {
    return false;
}

void SubsystemNull::set_music_volume(int v) { }

int SubsystemNull::get_music_volume() {
    return 0;
}

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
