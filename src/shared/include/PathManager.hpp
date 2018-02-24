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

#ifndef _PATHMANAGER_HPP_
#define _PATHMANAGER_HPP_

#include "Exception.hpp"

#include <string>

class PathManagerException : public Exception {
public:
    PathManagerException(const char *msg) : Exception(msg) { }
    PathManagerException(const std::string& msg) : Exception(msg) { }
};

class PathManager {
private:
    PathManager(const PathManager&);
    PathManager& operator=(const PathManager&);

public:
    PathManager(const char *application_name);

    const char *get_dir_separator() const;
    const char *get_data_home() const;
    const char *get_config_home() const;
    const char *get_cache_home() const;

private:
    std::string data_home;
    std::string config_home;
    std::string cache_home;
};

#endif
