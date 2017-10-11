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

#ifndef _ZIPWRITER_HPP_
#define _ZIPWRITER_HPP_

#include "Zip.hpp"

#include <string>
#include <vector>
#include <cstdio>

class ZipWriterException : public ZipException {
public:
    ZipWriterException(const char *msg) : ZipException(msg) { }
    ZipWriterException(const std::string& msg) : ZipException(msg) { }
};

class ZipWriter : public Zip {
private:
    ZipWriter(const ZipWriter&);
    ZipWriter& operator=(const ZipWriter&);

public:
    ZipWriter(const std::string& filename);
    virtual ~ZipWriter();

    void add_file(const std::string& filename);
    void remove_file(const std::string& filename);
};

#endif
