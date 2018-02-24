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

#ifndef _PROPERTIES_HPP_
#define _PROPERTIES_HPP_

#include "KeyValue.hpp"
#include "ZipReader.hpp"

#ifdef _WIN32
#include "Win.hpp"
#endif

class Properties : public KeyValue {
public:
    Properties();
    Properties(const KeyValue& kv);
    Properties(const std::string& filename, ZipReader *zip = 0);

    const std::string& get_name() const;
    const std::string& get_author() const;
    const std::string& get_description() const;

    void set_name(const std::string& name, bool no_touch = false);
    void set_author(const std::string& author, bool no_touch = false);
    void set_description(const std::string& description, bool no_touch = false);
    void reload_configuration();
    void save_configuration();

private:
    std::string filename;

    std::string name;
    std::string author;
    std::string description;

    void fetch();
};

#endif
