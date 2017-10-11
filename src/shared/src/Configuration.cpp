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

#include "Configuration.hpp"
#include "Utils.hpp"

#include <cstdlib>
#include <cstdio>

Configuration::Configuration(const std::string& directory, const std::string& filename)
    : save_at_exit(true)
{
    if (!filename.length()) {
        throw ConfigurationException("No valid configuration filename.");
    }

    std::string base_dir = get_home_directory();
    create_directory(directory, base_dir);
    this->filename = base_dir + "/" + directory + "/" + filename;

    /* load file */
    if (file_exists(this->filename)) {
        settings.read(this->filename);
    }

    /* load defaults */
    cdef("player_name", generate_name());
    cdef("show_player_name", "1");
    cdef("player_skin", "goat");
    cdef("fullscreen", "0");
    cdef("show_scanlines", "1");
    cdef("scanlines_intensity", "50");
    cdef("music_volume", "128");
    cdef("sfx_volume", "128");
    cdef("master_server", "master.goatattack.net");
    cdef("master_port", "25113");

    cdef("server_name", "frederic's temple");
    cdef("port", "25111");
    cdef("game_mode", "0");
    cdef("num_players", "12");
    cdef("duration", "20");
    cdef("warmup", "0");
    cdef("text_fade_speed", "6");
    cdef("reconnect_kills", "0");

    cdef("bind_escape_device", "0");
    cdef("bind_escape_param", "27");
    cdef("bind_chat_device", "0");
    cdef("bind_chat_param", "116");
    cdef("bind_down_device", "0");
    cdef("bind_down_param", "1073741905");
    cdef("bind_drop1_device", "0");
    cdef("bind_drop1_param", "115");
    cdef("bind_drop2_device", "0");
    cdef("bind_drop2_param", "100");
    cdef("bind_drop3_device", "0");
    cdef("bind_drop3_param", "102");
    cdef("bind_fire_device", "0");
    cdef("bind_fire_param", "97");
    cdef("bind_jump_device", "0");
    cdef("bind_jump_param", "32");
    cdef("bind_left_device", "0");
    cdef("bind_left_param", "1073741904");
    cdef("bind_right_device", "0");
    cdef("bind_right_param", "1073741903");
    cdef("bind_stats_device", "0");
    cdef("bind_stats_param", "9");
    cdef("bind_up_device", "0");
    cdef("bind_up_param", "1073741906");

    cdef("deadzone_horizontal", "3200");
    cdef("deadzone_vertical", "3200");

    /*
     * undocumented settings:
     * ----------------------
     * shading_pipeline=[0/1]  (use OpenGL 3.1 VBO streaming for drawing)
     * external_music=[string] (use external music for jukebox during gameplay)
     */

    /* extract -> ready for quick access */
    extract();
}

Configuration::~Configuration() {
    if (save_at_exit) {
        settings.save(filename);
    }
}

bool Configuration::do_save_at_exit() const {
    return save_at_exit;
}

void Configuration::set_save_at_exit(bool state) {
    save_at_exit = state;
}

std::string Configuration::get_string(const std::string& key) const {
    return settings.get_value(key);
}

void Configuration::set_string(const std::string& key, const std::string& value) {
    settings.set_value(key, value);
    extract();
}

int Configuration::get_int(const std::string& key) const {
    return atoi(settings.get_value(key).c_str());
}

void Configuration::set_int(const std::string& key, int value) {
    settings.set_value(key, value);
    extract();
}

bool Configuration::get_bool(const std::string& key) const {
    return (atoi(settings.get_value(key).c_str()) != 0);
}

void Configuration::set_bool(const std::string& key, bool value) {
    settings.set_value(key, value);
    extract();
}

float Configuration::get_float(const std::string& key) const {
    return static_cast<float>(atof(settings.get_value(key).c_str()));
}

void Configuration::set_float(const std::string& key, float value) {
    settings.set_value(key, value);
    extract();
}

bool Configuration::get_show_player_name() const {
    return show_player_name;
}

const std::string& Configuration::get_player_name() const {
    return player_name;
}

const std::string& Configuration::get_player_skin() const {
    return player_skin;
}

void Configuration::cdef(const std::string& key, const std::string& value) {
    if (!settings.get_value(key).length()) {
        settings.set_value(key, value);
    }
}

void Configuration::extract() {
    show_player_name = get_bool("show_player_name");
    player_name = get_string("player_name");
    player_skin = get_string("player_skin");
}

KeyValue& Configuration::get_key_value() {
    return settings;
}
