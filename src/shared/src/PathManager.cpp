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

#include "PathManager.hpp"
#include "Utils.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>

PathManager::PathManager(const char *application_name) {
    if (!application_name) {
        throw PathManagerException("no application name passed.");
    }

    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home) {
        data_home = std::string(xdg_data_home) + dir_separator + application_name;
    } else {
        data_home = get_home_directory() + dir_separator + ".local" + dir_separator + "share" + dir_separator + application_name;
    }

    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    if (xdg_config_home) {
        config_home = std::string(xdg_config_home) + dir_separator + application_name;
    } else {
        config_home = get_home_directory() + dir_separator + ".config" + dir_separator + application_name;
    }

    const char *xdg_cache_home = getenv("XDG_CACHE_HOME");
    if (xdg_cache_home) {
        cache_home = std::string(xdg_cache_home) + dir_separator + application_name;
    } else {
        cache_home = get_home_directory() + dir_separator + ".cache" + dir_separator + application_name;
    }

    create_complete_directory(data_home);
    create_complete_directory(config_home);
    create_complete_directory(cache_home);
}

const char *PathManager::get_dir_separator() const {
    return dir_separator;
}

const char *PathManager::get_data_home() const {
    return data_home.c_str();
}

const char *PathManager::get_config_home() const {
    return config_home.c_str();
}

const char *PathManager::get_cache_home() const {
    return cache_home.c_str();
}
