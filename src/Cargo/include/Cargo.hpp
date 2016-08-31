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

#ifndef _CARGO_HPP_
#define _CARGO_HPP_

#include "Exception.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <cstring>

class CargoException : public Exception {
public:
    CargoException(const char *msg) : Exception(msg) { }
    CargoException(std::string msg) : Exception(msg) { }
};

class Cargo {
private:
    Cargo(const Cargo&);
    Cargo& operator=(const Cargo&);

public:
    Cargo(const char *directory, const char *pak_file) throw (CargoException);
    ~Cargo();

    void pack() throw (CargoException);

private:
    struct DirectoryEntry {
        DirectoryEntry(bool is_directory, const char *entry)
            : is_directory(is_directory), entry(entry) { }

        bool is_directory;
        std::string entry;

        bool operator<(const DirectoryEntry& rhs) const {
            if (is_directory && !rhs.is_directory) {
                return true;
            }

            if (is_directory == rhs.is_directory) {
                return (strcmp(entry.c_str(), rhs.entry.c_str()) < 0);
            }

            return false;
        }
    };

    typedef std::vector<DirectoryEntry> DirectoryEntries;

    FILE *f;
    bool valid;
    std::string directory;

    void pack_directory(const char *subdir) throw (CargoException);
    void throw_write_error() throw (CargoException);
    std::string append_dir(const char *directory, const char *subdir);
};

#endif
