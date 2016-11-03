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

#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include "Exception.hpp"
#include "KeyValue.hpp"

#include <string>

class ConfigurationException : public Exception {
public:
    ConfigurationException(const char *msg) : Exception(msg) { }
    ConfigurationException(const std::string& msg) : Exception(msg) { }
};

class Configuration {
public:
    Configuration(const std::string& directory, const std::string& filename) throw (ConfigurationException);
    virtual ~Configuration();

    bool do_save_at_exit() const;
    void set_save_at_exit(bool state);

    std::string get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);

    int get_int(const std::string& key) const;
    void set_int(const std::string& key, int value);

    bool get_bool(const std::string& key) const;
    void set_bool(const std::string& key, bool value);

    float get_float(const std::string& key) const;
    void set_float(const std::string& key, float value);

    bool get_show_player_name() const;
    const std::string& get_player_name() const;
    const std::string& get_player_skin() const;

    void extract();

    KeyValue& get_key_value();

private:
    bool save_at_exit;
    std::string filename;
    KeyValue settings;
    char buffer[128];
    bool show_player_name;
    std::string player_name;
    std::string player_skin;

    void cdef(const std::string& key, const std::string& value);
};

#endif
