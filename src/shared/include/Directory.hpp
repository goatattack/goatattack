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

#ifndef _DIRECTORY_HPP_
#define _DIRECTORY_HPP_

#include "Exception.hpp"
#include "ZipReader.hpp"

#include <string>
#ifdef __unix__
#include <dirent.h>
#elif _WIN32
#include "Win.hpp"
#endif

class DirectoryException : public Exception {
public:
    DirectoryException(const char *msg) : Exception(msg) { }
    DirectoryException(const std::string& msg) : Exception(msg) { }
};

class Directory {
private:
    Directory(const Directory&);
    Directory& operator=(const Directory&);

public:
    Directory(const std::string& directory, const std::string& suffix, ZipReader *zip = 0);
    virtual ~Directory();

    const char *get_entry();

private:
    std::string directory;
    std::string suffix;
    bool finished;
    ZipReader *zip;
    Zip::Files::const_iterator zf_it;
    char zip_ret[256];
#ifdef __unix__
    DIR *dir;
#elif _WIN32
    bool first;
    HANDLE dir;
    WIN32_FIND_DATAW ffd;
    char directory_ret[MaxPathLength];
#endif
};

#endif
