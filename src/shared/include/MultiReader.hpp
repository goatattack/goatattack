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

#ifndef _MULTIREADER_HPP_
#define _MULTIREADER_HPP_

#include "Exception.hpp"
#include "Resources.hpp"
#include "ZipReader.hpp"

#include <string>

class MultiReaderException : public Exception {
public:
    MultiReaderException(const char *msg) : Exception(msg) { }
    MultiReaderException(const std::string& msg) : Exception(msg) { }
};

class MultiReader {
private:
    MultiReader(const MultiReader&);
    MultiReader& operator=(const MultiReader&);

public:
    MultiReader(const std::string& filename, ZipReader *zip);
    ~MultiReader();

    const std::string& get_filename() const;
    size_t get_size() const;
    size_t read(void *dest, size_t sz);

private:
    std::string filename;
    FILE *f;
    ZipReader *zip;
    const char *data;
    const char *ptr;
    size_t size;
};

#endif
