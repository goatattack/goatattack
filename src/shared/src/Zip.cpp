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

#include "Zip.hpp"
#include "CRC64.hpp"
#include "Utils.hpp"

#include <cstring>

Zip::Zip(const std::string& filename) : filename(filename), f(0) {
    size_t sz = filename.length();
    size_t pos = 0;
    for (int i = sz - 1; i >= 0; i--) {
        if (filename[i] == dir_separator[0]) {
            pos = i + strlen(dir_separator);
            break;
        }
    }
    this->short_filename.assign(&filename[pos], sz - pos);
}

Zip::~Zip() {
    if (f) {
        fclose(f);
    }
}

const std::string& Zip::get_zip_filename() const {
    return filename;
}

const std::string& Zip::get_zip_short_filename() const {
    return short_filename;
}

const std::string& Zip::get_hash() {
    if (!hash.length()) {
        rehash();
    }

    return hash;
}

void Zip::rehash() {
    if (!f) {
        throw ZipException("No file opened");
    }

    CRC64 crc;
    fseek(f, 0, SEEK_SET);
    while (true) {
        size_t sz = fread(buffer, 1, sizeof buffer, f);
        if (sz) {
            crc.process(buffer, sz);
        }
        if (!sz) {
            break;
        }
    }
    hash = crc.get_hash();
}
