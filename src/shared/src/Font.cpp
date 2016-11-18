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
#include "AutoPtr.hpp"
#include "UTF8.hpp"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

#ifdef __unix__
#include <arpa/inet.h>
#elif _WIN32
#include "Win.hpp"
#endif

static const int PageSize = 256;

Font::Font(Subsystem& subsystem, FT_Library& ft, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, FontException)
    : Properties(filename + ".font", zip), subsystem(subsystem),
      i18n(subsystem.get_i18n()), ft(ft), start_page(create_new_page()),
      kerning(false), outline_monochrome(false), monochrome(false),
      outline_alpha_factor(1.0), alpha_factor(1.0), total_height(0)
{
    try {
        const std::string& fontfile(get_value("font"));
        if (!fontfile.length()) {
            throw FontException("no file file declared in " + filename);
        }

        /* get base informations */
        width = atoi(get_value("width").c_str());
        height = atoi(get_value("height").c_str());
        total_height = height;
        float outline = atof(get_value("outline").c_str());
        y_offset = atoi(get_value("y_offset").c_str());
        outline_monochrome = (atoi(get_value("outline_monochrome").c_str()) ? true : false);
        monochrome = (atoi(get_value("monochrome").c_str()) ? true : false);

        /* color gradient */
        red1 = static_cast<char>(atoi(get_value("red1").c_str()));
        green1 = static_cast<char>(atoi(get_value("green1").c_str()));
        blue1 = static_cast<char>(atoi(get_value("blue1").c_str()));
        red2 = static_cast<char>(atoi(get_value("red2").c_str()));
        green2 = static_cast<char>(atoi(get_value("green2").c_str()));
        blue2 = static_cast<char>(atoi(get_value("blue2").c_str()));

        /* alpha factors */
        const std::string& soaf(get_value("outline_alpha_factor"));
        const std::string& af(get_value("alpha_factor"));
        if (soaf.length()) {
            outline_alpha_factor = atof(soaf.c_str());
        }
        if (af.length()) {
            alpha_factor = atof(af.c_str());
        }

        /* read font file from file or zip */
        MultiReader ff("fonts/" + fontfile, zip);
        AutoPtr<FT_Byte[]> tmpbuf(new FT_Byte[ff.get_size()]);
        ff.read(&tmpbuf[0], ff.get_size());

        /* load font into freetype */
        if (FT_New_Memory_Face(ft, &tmpbuf[0], static_cast<FT_Long>(ff.get_size()), 0, &face)) {
            throw FontException(std::string("loading font '") + ff.get_filename() + "' failed.");
        }

        /* setup encoding and pixel size */
        FT_Select_Charmap(face, ft_encoding_unicode);
        FT_Set_Pixel_Sizes(face, width, height);
        kerning = FT_HAS_KERNING(face);
        FT_Stroker_New(ft, &stroker);
        FT_Stroker_Set(stroker, static_cast<FT_Fixed>(outline * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

        /* add two pixels (top/bottom) */
        int overridden_height = atoi(get_value("overridden_height").c_str());
        if (overridden_height) {
            height = overridden_height;
        }
        height += 2;

        /* done */
        font_buffer = tmpbuf.release();
    } catch (const Exception& e) {
        throw FontException(e.what());
    }
}

Font::~Font() {
    FT_Stroker_Done(stroker);
    FT_Done_Face(face);
    delete_pages(start_page);
    delete[] font_buffer;
}

int Font::get_font_height() const {
    return height;
}

int Font::get_font_total_height() const {
    return total_height;
}

int Font::get_text_width(const char *s) {
    const Font::Character *prev = 0;

    int w = 0;
    while (*s) {
        const Font::Character *chr = get_character(s);
        w += chr->advance + get_x_kerning(prev, chr);
        s += chr->distance;
        prev = chr;
    }

    return w;
}

int Font::get_text_width(const std::string& text) {
    size_t sz = text.length();
    const Font::Character *prev = 0;

    int w = 0;
    if (sz) {
        const char *p = text.c_str();
        while (*p) {
            const Font::Character *chr = get_character(p);
            w += chr->advance + get_x_kerning(prev, chr);
            p += chr->distance;
            prev = chr;
        }
    }
    return w;
}

int Font::get_char_width(const char *s) {
    return get_character(s)->advance;
}

int Font::get_y_offset() const {
    return y_offset;
}

int Font::get_x_kerning(const Character *prev, const Character *cur) {
    if (kerning && prev && cur) {
        FT_Vector d;
        FT_Get_Kerning(face, prev->glyph_index, cur->glyph_index, FT_KERNING_DEFAULT, &d);
        return static_cast<int>(d.x >> 6);
    }

    return 0;
}

void Font::clip_on(int x, int y, int width, int height) {
    subsystem.enable_cliprect(x, y + total_height + y_offset, width, height);
}

void Font::clip_on(int x, int y, int width) {
    clip_on(x, y, width, height);
}

void Font::clip_off() {
    subsystem.disable_cliprect();
}

Font::Data *Font::create_new_page() {
    Data *page = new Data[PageSize];
    memset(page, 0, sizeof(Data) * PageSize);

    return page;
}

void Font::delete_pages(Data *page) {
    if (page) {
        for (int i = 0; i < PageSize; i++) {
            Data& data = page[i];
            delete_pages(data.next);
            if (data.chr && data.chr->tile) {
                delete data.chr->tile;
            }
            delete data.chr;
        }
        delete[] page;
    }
}

const Font::Character *Font::get_character(const char *s) {
    while (true) {
        const unsigned char *p = reinterpret_cast<const unsigned char *>(s);
        Data *page = start_page;
        while (true) {
            const unsigned char c = *p++;
            Data& data = page[c];
            if (data.chr) {
                return data.chr;
            }
            page = data.next;
            if (!page) {
                create_character(s);
                break;
            }
        }
    }
}

void Font::create_character(const char *s) {
    Data *page = start_page;
    int state = UTF8_ACCEPT;
    uint32_t codepoint = 0;
    int distance = 1;
    char buffer[64];
    const unsigned char *p = reinterpret_cast<const unsigned char *>(s);
    while (true) {
        const unsigned char c = *p;
        Data& data = page[c];
        if (!utf8_decode(c, state, codepoint) || !c) {
            /* create glyph bitmap from char index */
            FT_UInt glyph_index = FT_Get_Char_Index(face, codepoint);
            FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            unsigned int fw = 0;
            unsigned int fh = 0;
            unsigned char *tmppic = 0;
            for (int pass = 0; pass < 2; pass++) {
                FT_Bitmap mono_bitmap;
                FT_Bitmap_New(&mono_bitmap);
                FT_Glyph glyph;
                FT_Get_Glyph(face->glyph, &glyph);
                FT_BitmapGlyph bitmap_glyph;
                bool do_monochrome = false;
                if (pass == 0) {
                    FT_Glyph_StrokeBorder(&glyph, stroker, false, true);
                }
                if ((pass == 0 && outline_monochrome) || (pass == 1 && monochrome)) {
                    FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, true);
                    bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
                    FT_Bitmap_Convert(ft, &bitmap_glyph->bitmap, &mono_bitmap, 1);
                    do_monochrome = true;
                } else {
                    FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, true);
                    bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
                }

                /* create temporary white alpha picture from 8bit glyph bmp */
                unsigned int sz = bitmap_glyph->bitmap.width * bitmap_glyph->bitmap.rows;
                if (pass == 0) {
                    /* create outline */
                    fw = bitmap_glyph->bitmap.width;
                    fh = bitmap_glyph->bitmap.rows;
                    tmppic = new unsigned char[sz * 4];
                    unsigned char *dst = tmppic;
                    unsigned char *src = (do_monochrome ? mono_bitmap.buffer : bitmap_glyph->bitmap.buffer);
                    int alpha = 0;
                    for (unsigned int i = 0; i < sz; i++) {
                        *dst++ = 0; *dst++ = 0; *dst++ = 0;
                        if (do_monochrome) {
                            alpha = (*src++ ? 255 : 0);
                        } else {
                            alpha = static_cast<int>(static_cast<int>(*src++) * outline_alpha_factor);
                        }
                        *dst++ = static_cast<unsigned char>(alpha > 255 ? 255 : alpha);
                    }
                    /* create character */
                    data.chr = new Character;
                    data.chr->glyph_index = glyph_index;
                    data.chr->width = static_cast<int>(bitmap_glyph->bitmap.width);
                    data.chr->rows = static_cast<int>(bitmap_glyph->bitmap.rows);
                    data.chr->left = static_cast<int>(bitmap_glyph->left);
                    data.chr->top = static_cast<int>(bitmap_glyph->top);
                    data.chr->y_offset = -data.chr->top + height;
                    data.chr->advance = static_cast<int>(face->glyph->advance.x) >> 6;
                    data.chr->distance = distance;
                    /* add manually tweaked x advance in font file */
                    sprintf(buffer, "cp_%d_x_shift", codepoint);
                    data.chr->advance += atoi(get_value(buffer).c_str());
                } else {
                    /* alpha blend font over outline */
                    unsigned char *dst = tmppic;
                    unsigned char *src = (do_monochrome ? mono_bitmap.buffer : bitmap_glyph->bitmap.buffer);
                    int y_diff = (fh - bitmap_glyph->bitmap.rows);
                    int yt_diff = y_diff - (fh - bitmap_glyph->bitmap.rows) / 2;
                    int x_diff = (fw - bitmap_glyph->bitmap.width);
                    int xl_diff = x_diff / 2;
                    int xr_diff = x_diff - xl_diff;

                    unsigned int h = bitmap_glyph->bitmap.rows;
                    double gradient_h = static_cast<double>(h > 1 ? h - 1 : 1);
                    double red = static_cast<double>(red1);
                    double green = static_cast<double>(green1);
                    double blue = static_cast<double>(blue1);
                    double red_gradient = static_cast<double>(red2 - red1) / gradient_h;
                    double green_gradient = static_cast<double>(green2 - green1) / gradient_h;
                    double blue_gradient = static_cast<double>(blue2 - blue1) / gradient_h;

                    /* add manually tweaked blending x offset in font file */
                    sprintf(buffer, "cp_%d_blend_x_shift", codepoint);
                    int blend_offset = atoi(get_value(buffer).c_str());
                    xl_diff += blend_offset;
                    xr_diff -= blend_offset;

                    /* blend */
                    dst += yt_diff * fw * 4;
                    unsigned char alpha = 0;
                    for (unsigned int y = 0; y < h; y++) {
                        dst += xl_diff * 4;
                        for (unsigned int x = 0; x < bitmap_glyph->bitmap.width; x++) {
                            if (do_monochrome) {
                                alpha = (*src++ ? 255 : 0);
                            } else {
                                int sharp_alpha = static_cast<int>(static_cast<int>(*src++) * alpha_factor);
                                alpha = (sharp_alpha > 255 ? 255 : sharp_alpha);
                            }
                            dst[0] = ((alpha * (static_cast<unsigned char>(red) - dst[0])) >> 8) + dst[0];
                            dst[1] = ((alpha * (static_cast<unsigned char>(green) - dst[1])) >> 8) + dst[1];
                            dst[2] = ((alpha * (static_cast<unsigned char>(blue) - dst[2])) >> 8) + dst[2];
                            dst += 4;
                        }
                        dst += xr_diff * 4;
                        red += red_gradient;
                        green += green_gradient;
                        blue += blue_gradient;
                    }

                    /* add tile to character */
                    TileGraphic *tg = subsystem.create_tilegraphic(fw, fh);
                    tg->add_tile(4, &tmppic[0], false, false);
                    data.chr->tile = new Tile(tg, false, Tile::TileTypeNonblocking, 0, false, 0.0f);
                    delete[] tmppic;
                }
                FT_Bitmap_Done(ft, &mono_bitmap);
                FT_Done_Glyph(glyph);
            }
            break;
        } else {
            if (!data.next) {
                data.next = create_new_page();
            }
            page = data.next;
            distance++;
        }
        p++;
    }
}
