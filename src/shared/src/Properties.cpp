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

#include "Properties.hpp"

Properties::Properties() { }

Properties::Properties(const KeyValue& kv) : KeyValue(kv) { }

Properties::Properties(const std::string& filename, ZipReader *zip)
    : KeyValue(filename, zip), filename(filename)
{
    fetch();
    if (name.length() > 31) {
        throw KeyValueException("Property name too long. Must be less than 32. (" + filename + ")");
    }
}

Properties::~Properties() { }

const std::string& Properties::get_name() const {
    return name;
}

const std::string& Properties::get_author() const {
    return author;
}

const std::string& Properties::get_description() const {
    return description;
}

void Properties::set_name(const std::string& name, bool no_touch) {
    set_value("name", name, no_touch);
    this->name = name;
    if (!no_touch) {
        touch();
    }
}

void Properties::set_author(const std::string& author, bool no_touch) {
    set_value("author", author, no_touch);
    this->author = author;
    if (!no_touch) {
        touch();
    }
}

void Properties::set_description(const std::string& description, bool no_touch) {
    set_value("description", description, no_touch);
    this->description = description;
    if (!no_touch) {
        touch();
    }
}

void Properties::reload_configuration() {
    if (!filename.length()) {
        throw KeyValueException("No filename specified");
    }
    clear();
    read(filename);
    fetch();
}

void Properties::save_configuration() {
    if (!filename.length()) {
        throw KeyValueException("No filename specified");
    }
    save(filename);
}

void Properties::fetch() {
    name = get_value("name");
    author = get_value("author");
    description = get_value("description");
}
