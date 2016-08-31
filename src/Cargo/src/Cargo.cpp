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

#include "Cargo.hpp"
#include "Directory.hpp"

#include <cerrno>
#include <algorithm>
#include <sys/stat.h>

#include <iostream>

static bool is_directory(const char *entry) {
    struct stat sinfo;
    return (stat(entry, &sinfo) == 0 && S_ISDIR(sinfo.st_mode));
}

Cargo::Cargo(const char *directory, const char *pak_file) throw (CargoException)
    : f(0), valid(true), directory(directory)
{
    f = fopen(pak_file, "wb");
    if (!f) {
        throw CargoException("Cannot open file: " + std::string(strerror(errno)));
    }
}

Cargo::~Cargo() {
    fclose(f);
}

void Cargo::pack() throw (CargoException) {
    /* object invalid? */
    if (!valid) {
        throw CargoException("Object is corrupt.");
    }

    /* go */
    pack_directory("");

    /* flush file */
    if (fflush(f)) {
        throw_write_error();
    }
}

void Cargo::pack_directory(const char *subdir) throw (CargoException) {
    try {
        DirectoryEntries entries;
        std::string root = directory + "/" + subdir;

        Directory dir(root, "", 0);
        const char *entry = 0;
        while ((entry = dir.get_entry())) {
            if (strcmp(entry, ".") && strcmp(entry, "..")) {
                std::string new_entry = append_dir(subdir, entry);
                std::string test = append_dir(directory.c_str(), new_entry.c_str());

                entries.push_back(DirectoryEntry(is_directory(test.c_str()), new_entry.c_str()));
            }
        }
        std::sort(entries.begin(), entries.end());

        for (DirectoryEntries::iterator it = entries.begin(); it != entries.end(); it++) {
            const DirectoryEntry& entry = *it;
            if (entry.is_directory) {
                pack_directory(append_dir(subdir, entry.entry.c_str()).c_str());
            } else {
                /* add to zip here */
                std::cout << entry.entry << std::endl;
            }
        }

    } catch (const Exception& e) {
        valid = false;
        throw CargoException(e.what());
    }
}

void Cargo::throw_write_error() throw (CargoException) {
    valid = false;
    throw CargoException("Writing to file failed: " + std::string(strerror(errno)));
}

std::string Cargo::append_dir(const char *directory, const char *subdir) {
    std::string new_dir(directory);
    if (new_dir.length()) {
        new_dir += "/";
    }
    new_dir += subdir;

    return new_dir;
}