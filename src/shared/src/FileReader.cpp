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

#include "FileReader.hpp"

FileReader::FileReader(const char *filename) throw (FileReaderException) {
    f = fopen(filename, "rb");
    if (!f) {
        throw FileReaderException("Opening file failed.");
    }
}

size_t FileReader::read(void *buffer, size_t len) throw (FileReaderException) {
    return fread(buffer, 1, len, f);
}

bool FileReader::eof() const {
    return (feof(f) != 0);
}