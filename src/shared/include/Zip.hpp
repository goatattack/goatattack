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

#ifndef _ZIP_HPP_
#define _ZIP_HPP_

#include "Exception.hpp"

#include <zlib.h>

#include <string>
#include <vector>
#include <cstdio>

class ZipException : public Exception {
public:
    ZipException(const char *msg) : Exception(msg) { }
    ZipException(const std::string& msg) : Exception(msg) { }
};

class Zip {
private:
    Zip(const Zip&);
    Zip& operator=(const Zip&);

public:
    struct File {
        std::string filename;
        size_t sz;
        size_t ofs;
    };

    typedef std::vector<File> Files;

    Zip(const std::string& filename);
    virtual ~Zip();

    const std::string& get_zip_filename() const;
    const std::string& get_zip_short_filename() const;
    const std::string& get_hash();
    void rehash();

protected:
    std::string filename;
    std::string short_filename;
    FILE *f;
    std::string hash;

    Files files;
    unsigned char buffer[16384];
    unsigned char outbuf[16384];
};

#endif
