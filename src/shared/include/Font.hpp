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

#ifndef _FONT_HPP_
#define _FONT_HPP_

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"
#include "FT.hpp"

class FontException : public Exception {
public:
    FontException(const char *msg) : Exception(msg) { }
    FontException(const std::string& msg) : Exception(msg) { }
};

static const int FontMin = 32;
static const int FontMax = 128;
static const int NumOfChars = FontMax - FontMin;

typedef struct font_char {
    int32_t origin_x;
    int32_t origin_y;
    int32_t width;
    int32_t height;
} font_char_t;

class Font : public Properties {
private:
    Font(const Font&);
    Font& operator=(const Font&);

public:
    struct Character {
        FT_UInt glyph_index;
        int width;
        int rows;
        int left;
        int top;
        int y_offset;
        int advance;
        int distance;
        Tile *tile;
    };

    Font(Subsystem& subsystem, FT_Library& ft, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, FontException);
    virtual ~Font();

    const Character *get_character(const char *s);
    Tile *get_tile(int index);
    int get_fw(int index);
    int get_spacing();
    int get_font_height();
    int get_text_width(const std::string& text);
    int get_char_width(unsigned char c);
    int get_y_offset() const;
    int get_x_kerning(const Character *prev, const Character *cur);

private:
    struct Data {
        Data *next;
        Character *chr;
    };

    Subsystem& subsystem;
    I18N& i18n;
    FT_Library& ft;
    int max_height;
    Data *start_page;
    bool kerning;
    double outline_alpha_factor;
    double alpha_factor;

    unsigned int width;
    unsigned int height;
    unsigned int outline;
    unsigned char red1;
    unsigned char green1;
    unsigned char blue1;
    unsigned char red2;
    unsigned char green2;
    unsigned char blue2;
    int y_offset;
    FT_Face face;
    FT_Stroker stroker;
    FT_Byte *font_buffer;

    /* old */
    int offset;
    Tile *tiles[NumOfChars];
    int fw[NumOfChars];
    int fh[NumOfChars];
    int32_t font_height;
    int spacing;

    Data *create_new_page();
    void delete_pages(Data *page);
    void create_character(const char *s);
};

#endif
