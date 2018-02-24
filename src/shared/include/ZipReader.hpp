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

#ifndef _ZIPREADER_HPP_
#define _ZIPREADER_HPP_

#include "Zip.hpp"

#include <string>
#include <vector>
#include <cstdio>

class ZipReaderException : public ZipException {
public:
    ZipReaderException(const char *msg) : ZipException(msg) { }
    ZipReaderException(const std::string& msg) : ZipException(msg) { }
};

class ZipReader : public Zip {
private:
    ZipReader(const ZipReader&);
    ZipReader& operator=(const ZipReader&);

public:
    ZipReader(const std::string& filename);

    const Files& get_files() const;
    bool file_exists(std::string filename);
    bool equals_directory(const File& file, const std::string& directory);
    const char *extract(std::string filename, size_t *out_sz = 0);
    static void destroy(const char *data);

private:
    void throw_corrupt_file(const std::string& filename);
    void throw_inflate_failed(z_stream *z, const char *data, const std::string& msg);
    const File& get_file(const std::string& filename);
};

#endif
