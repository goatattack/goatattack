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

#include "Font.hpp"
#include "PNG.hpp"
#include "MultiReader.hpp"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

#ifdef __unix__
#include <arpa/inet.h>
#elif _WIN32
#include "Win.hpp"
#endif

Font::Font(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, FontException)
    : Properties(filename + ".font", zip), subsystem(subsystem)
{
    try {
        PNG png(filename + ".png", zip);
        MultiReader mr(filename + ".fds", zip);

        char header[4];
        mr.read(header, sizeof(header));
        if (memcmp(header, "FNT1", 4)) {
            throw FontException("wrong font file " + mr.get_filename());
        }
        mr.read(&font_height, sizeof font_height);
        font_height = ntohl(font_height) * 2;
        spacing = atoi(get_value("spacing").c_str());

        font_char_t font;
        for (int i = 0; i < NumOfChars; i++) {
            mr.read(&font, sizeof font);
            font.origin_x = ntohl(font.origin_x);
            font.origin_y = ntohl(font.origin_y);
            font.width = ntohl(font.width);
            font.height = ntohl(font.height);

            TileGraphic *tg = subsystem.create_tilegraphic(font.width, font.height);
            tg->punch_out_tile(png, font.origin_x, font.origin_y, false);
            tiles[i] = new Tile(tg, false, Tile::TileTypeNonblocking, 0, false, 0.0f);
            fw[i] = font.width;
            fh[i] = font.height;
        }
    } catch (const Exception& e) {
        throw FontException(e.what());
    }
}

Font::~Font() {
    for (int i = 0; i < NumOfChars; i++) {
        delete tiles[i];
    }
}

Tile *Font::get_tile(int index) {
    return tiles[index];
}

int Font::get_fw(int index) {
    return fw[index];
}

int Font::get_spacing() {
    return spacing;
}

int Font::get_font_height() {
    return static_cast<int>(font_height);
}

int Font::get_text_width(const std::string& text) {
    size_t sz = text.length();

    int w = 0;
    for (size_t i = 0; i < sz; i++) {
        int c = text[i];
        if (c >= FontMin && c <= FontMax) {
            c -= FontMin;
            w += fw[c] + spacing;
        }
    }

    return w;
}

int Font::get_char_width(unsigned char c) {
    int w = 0;

    if (c >= FontMin && c <= FontMax) {
        c -= FontMin;
        w += fw[c] + spacing;
    }

    return w;
}