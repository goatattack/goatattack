#include "Configuration.hpp"
#include "Utils.hpp"

#include <cstdlib>
#include <cstdio>

Configuration::Configuration(const std::string& directory,
    const std::string& filename) throw (ConfigurationException)
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
    cdef("server_port", "25111");
    cdef("game_mode", "0");
    cdef("max_players", "12");
    cdef("duration", "20");
    cdef("warmup", "0");

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

    /* extract -> ready for quick access */
    extract();
}

Configuration::~Configuration() {
    settings.save(filename);
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
