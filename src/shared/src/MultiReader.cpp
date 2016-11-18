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

#include "MultiReader.hpp"

#include <cerrno>
#include <cstring>

MultiReader::MultiReader(const std::string& filename, ZipReader *zip) throw (MultiReaderException)
    : filename(filename), f(0), zip(zip), data(0), ptr(0), size(0)
{
    if (zip) {
        try {
            data = ptr = zip->extract(filename, &size);
        } catch (const Exception& e) {
            throw MultiReaderException(e.what());
        }
    } else {
        f = fopen(filename.c_str(), "rb");
        if (!f) {
            throw MultiReaderException("Cannot open file: " + filename + std::string(strerror(errno)));
        }
        fseek(f, SEEK_END, 0);
        size = static_cast<size_t>(ftell(f));
        fseek(f, SEEK_SET, 0);
    }
}

MultiReader::~MultiReader() {
    if (f) {
        fclose(f);
    }
    if (zip) {
        zip->destroy(data);
    }
}

const std::string& MultiReader::get_filename() const {
    return filename;
}

size_t MultiReader::get_size() const {
    return size;
}

size_t MultiReader::read(void *dest, size_t sz) {
    if (zip) {
        size_t remain = size - (ptr - data);
        if (sz > remain) {
            sz = remain;
        }
        memcpy(dest, ptr, sz);
        ptr += sz;
    } else {
        sz = fread(dest, 1, sz, f);
    }

    return sz;
}
