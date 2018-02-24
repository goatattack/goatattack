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

#ifndef _KEYVALUE_HPP_
#define _KEYVALUE_HPP_

#include "Exception.hpp"
#include "Utils.hpp"
#include "ZipReader.hpp"

#include <map>

#ifdef _WIN32
#include "Win.hpp"
#endif

class KeyValueException : public Exception {
public:
    KeyValueException(const char *msg) : Exception(msg) { }
    KeyValueException(const std::string& msg) : Exception(msg) { }
};

class KeyValue {
public:
    typedef std::map<std::string, std::string> Entries;

    KeyValue();
    KeyValue(const std::string& filename, ZipReader *zip = 0);

    const std::string& get_value(const std::string& key) const;
    void set_value(const std::string& key, const std::string& value, bool no_touch = false);
    void set_value(const std::string& key, const char *value, bool no_touch = false);
    void set_value(const std::string& key, int value, bool no_touch = false);
    void set_value(const std::string& key, double value, bool no_touch = false);
    void set_value(const std::string& key, bool value, bool no_touch = false);
    void read(const std::string& filename, ZipReader *zip = 0);
    void save(const std::string& filename);
    bool can_be_saved();
    void save_test();
    bool is_modified() const;
    void touch();
    void untouch();
    const Entries& get_entries() const;
    void clear();
    uint32_t get_hash_value() const;
    bool is_zip_file() const;
    const std::string& get_zip_filename() const;
    const std::string& get_zip_file_hash() const;

private:
    bool modified;
    int depth_counter;
    uint32_t hash_value;
    bool zip_file;

    Entries entries;
    std::string empty_string;
    std::string zip_filename;
    std::string zip_file_hash;

    void modify_zip_file_test();
    void process_line(std::string line, const std::string& filename, ZipReader *zip);
};

#endif
