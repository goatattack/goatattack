#include "Font.hpp"
#include "PNG.hpp"

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
        /* setup font operations */
        FontOperations fo;
        if (zip) {
            fo.handle = zip;
            fo.open = &fo_zip_open;
            fo.read = &fo_zip_read;
            fo.close = &fo_zip_close;
        } else {
            fo.handle = 0;
            fo.open = &fo_file_open;
            fo.read = &fo_file_read;
            fo.close = &fo_file_close;
        }

        std::string font_description = filename + ".fds";
        fo.open(fo, font_description);

        char header[4];
        fo.read(fo, header, sizeof(header));
        if (memcmp(header, "FNT1", 4)) {
            throw FontException("wrong font file " + font_description);
        }
        fo.read(fo, &font_height, sizeof font_height);
        font_height = ntohl(font_height) * 2;
        spacing = atoi(get_value("spacing").c_str());

        font_char_t font;
        for (int i = 0; i < NumOfChars; i++) {
            fo.read(fo, &font, sizeof font);
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

void Font::fo_file_open(FontOperations& op, const std::string& filename) throw (FontException) {
    op.handle = fopen(filename.c_str(), "rb");

    if (!op.handle) {
        throw FontException("cannot open font file " + filename + ": "
            + std::string(strerror(errno)));
    }
}

void Font::fo_file_read(FontOperations& op, void *data, size_t sz) {
    if (op.handle) {
        fread(data, 1, sz, static_cast<FILE *>(op.handle));
    }
}

void Font::fo_file_close(FontOperations& op) {
    if (op.handle) {
        fclose(static_cast<FILE *>(op.handle));
        op.handle = 0;
    }
}

void Font::fo_zip_open(FontOperations& op, const std::string& filename) throw (FontException) {
    if (!op.data) {
        ZipReader *zip = static_cast<ZipReader *>(op.handle);
        size_t sz;
        op.data = op.ptr = zip->extract(filename, &sz);
        op.size = static_cast<size_t>(sz);
    } else {
        throw FontException("FontOperations handle already in use");
    }
}

void Font::fo_zip_read(FontOperations& op, void *data, size_t sz) {
    if (op.handle) {
        const char *dta = op.data;
        const char *ptr = op.ptr;
        size_t remain = op.size - (ptr - dta);
        if (sz > remain) {
            sz = remain;
        }
        memcpy(data, ptr, sz);
        ptr += sz;
        op.ptr = ptr;
    }
}

void Font::fo_zip_close(FontOperations& op) {
    if (op.handle && op.data) {
        ZipReader *zip = static_cast<ZipReader *>(op.handle);
        zip->destroy(op.data);
        op.data = 0;
    }
}