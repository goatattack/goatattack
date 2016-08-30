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

#ifndef _PNG_HPP_
#define _PNG_HPP_

#include "Exception.hpp"
#include "ZipReader.hpp"

#ifdef __APPLE__
#include "png.h"
#else
#include <png.h>
#endif

#ifdef _WIN32
#include "Win.hpp"
#endif

class PNGException : public Exception {
public:
    PNGException(const char *msg) : Exception(msg) { }
    PNGException(const std::string& msg) : Exception(msg) { }
};

class PNG {
private:
    PNG(const PNG&);
    PNG& operator=(const PNG&);

public:
    enum ColorFormat {
        ColorFormatRGB,
        ColorFormatRGBA
    };

    PNG(const std::string& filename, ZipReader *zip = 0) throw (PNGException);
    virtual ~PNG();

    void flip_h();
    void flip_v();

    unsigned int get_width() const;
    unsigned int get_height() const;
    int get_bit_depth() const;
    ColorFormat get_color_format() const;
    unsigned char *get_pic() const;

private:
    struct PNGZipStream {
        PNGZipStream (ZipReader *zip) : zip(zip), size(0), data(0), ptr(0) { }

        ZipReader *zip;
        size_t size;
        const char *data;
        const char *ptr;
    };

    unsigned int width;
    unsigned int height;
    int bit_depth;
    ColorFormat color_format;
    unsigned char *pic;

    void read_png_from_file(const std::string& filename) throw (PNGException);
    void read_png_from_zip(const std::string& filename, ZipReader *zip) throw (PNGException);

    static void user_data_read(png_structp png_ptr, png_bytep data, png_size_t len);
};

#endif
