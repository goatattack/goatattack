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
#include "CRC64.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <stdint.h>

class CargoException : public Exception {
public:
    CargoException(const char *msg) : Exception(msg) { }
    CargoException(std::string msg) : Exception(msg) { }
};

class SelectedFiles {
    friend class Cargo;

public:
    typedef std::vector<std::string> Files;

    void push(const char *filename);
    const Files& get_files() const;

private:
    Files files;
};

class Cargo {
private:
    Cargo(const Cargo&);
    Cargo& operator=(const Cargo&);

public:
    Cargo(const char *directory, const char *pak_file, const SelectedFiles *files = 0) throw (CargoException);
    ~Cargo();

    static bool file_exists(const std::string& filename);

    void pack() throw (CargoException);
    size_t packaged() const;
    std::string get_hash() const;


private:
    struct DirectoryEntry {
        DirectoryEntry(bool is_directory, const char *entry, size_t file_size)
            : is_directory(is_directory), entry(entry), file_size(file_size) { }

        bool is_directory;
        std::string entry;
        size_t file_size;

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

    struct PakEntry {
        PakEntry(const char *name, uint32_t sz_compr, uint32_t sz_uncompr,
            uint32_t rel_pos, uint32_t crc32, uint16_t file_time,
            uint16_t file_date, uint16_t method)
            : name(name), sz_compressed(sz_compr), sz_uncompressed(sz_uncompr),
              relative_position(rel_pos), crc32(crc32), file_time(file_time),
              file_date(file_date), method(method) { }

        std::string name;
        uint32_t sz_compressed;
        uint32_t sz_uncompressed;
        uint32_t relative_position;
        uint32_t crc32;
        uint16_t file_time;
        uint16_t file_date;
        uint16_t method;
    };

    typedef std::vector<PakEntry> PakEntries;

    FILE *f;
    bool has_selected_files;
    bool valid;
    bool finished;
    std::string directory;
    std::string pak_file;

    SelectedFiles selected_files;
    PakEntries pak_entries;
    CRC64 crc64;

    void pack_directory(const char *subdir, bool is_rootdir = false) throw (CargoException);
    void pack_file(const DirectoryEntry& entry) throw (CargoException);
    void throw_write_error() throw (CargoException);
    std::string append_dir(const char *directory, const char *subdir);
    void add_central_directory() throw (CargoException);
    size_t write_string(const void *s, size_t len) throw (CargoException);
    size_t write_uint16(uint16_t n) throw (CargoException);
    size_t write_uint32(uint32_t n) throw (CargoException);
    void calc_crc64(size_t start_pos, size_t end_pos) throw (CargoException);
};

#endif
