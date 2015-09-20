#include "PNG.hpp"

#include <cstdio>
#include <cstring>
#include <cerrno>

PNG::PNG(const std::string& filename, ZipReader *zip) throw (PNGException) {
    if (zip) {
        read_png_from_zip(filename, zip);
    } else {
        read_png_from_file(filename);
    }
}

PNG::~PNG() {
    delete[] pic;
}

void PNG::flip_h() {
    unsigned int bytes_per_pixel = 0;
    switch (color_format) {
        case ColorFormatRGB:
            bytes_per_pixel = 3;
            break;

        case ColorFormatRGBA:
            bytes_per_pixel = 4;
            break;
    }

    if (bytes_per_pixel) {
        unsigned int row_bytes = width * bytes_per_pixel;
        unsigned char *new_pic = new unsigned char[row_bytes * height];

        unsigned char *src = pic;
        for (unsigned int y = 0; y < height; y++) {
            unsigned char *dest = new_pic + (y * row_bytes);
            dest += (width - 1) * bytes_per_pixel;
            for (unsigned int x = 0; x < width; x++) {
                for (unsigned int px = 0; px < bytes_per_pixel; px++) {
                    *dest++ = *src++;
                }
                dest -= 2 * bytes_per_pixel;
            }
        }

        delete[] pic;
        pic = new_pic;
    }
}

void PNG::flip_v() {
    unsigned int bytes_per_pixel = 0;
    switch (color_format) {
        case ColorFormatRGB:
            bytes_per_pixel = 3;
            break;

        case ColorFormatRGBA:
            bytes_per_pixel = 4;
            break;
    }

    if (bytes_per_pixel) {
        unsigned int row_bytes = width * bytes_per_pixel;
        unsigned char *new_pic = new unsigned char[row_bytes * height];

        unsigned char *src = pic;
        for (unsigned int y = 0; y < height; y++) {
            unsigned char *dest = new_pic + ((height - 1 - y) * row_bytes);
            for (unsigned int x = 0; x < width; x++) {
                for (unsigned int px = 0; px < bytes_per_pixel; px++) {
                    *dest++ = *src++;
                }
            }
        }

        delete[] pic;
        pic = new_pic;
    }
}

unsigned int PNG::get_width() const {
    return width;
}

unsigned int PNG::get_height() const {
    return height;
}

int PNG::get_bit_depth() const {
    return bit_depth;
}

PNG::ColorFormat PNG::get_color_format() const {
    return color_format;
}

unsigned char *PNG::get_pic() const {
    return pic;
}

void PNG::read_png_from_file(const std::string& filename) throw (PNGException) {
    FILE *f;
    png_structp png_ptr;
    png_infop info_ptr;

    /* try to open file */
    f = fopen(filename.c_str(), "rb");
    if (!f) {
        throw PNGException("Cannot open PNG file " + filename + ": " +
            std::string(strerror(errno)));
    }

    /* check header */
    unsigned char header[8];
    fread(header, 1, 8, f);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(f);
        throw PNGException("File is not recognized as PNG file: " + filename);
    }

    /* initialize png */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) {
        fclose(f);
        throw PNGException("png_create_read_struct() failed");
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, 0, 0);
        fclose(f);
        throw PNGException("png_create_info_struct() failed");
    }

    /* setup error handler */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, 0, 0);
        fclose(f);
        throw PNGException("png_create_info_struct() failed");
    }

    /* read header informations */
    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, 8);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    png_uint_32 _width;
    png_uint_32 _height;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &_width, &_height, &bit_depth, &color_type,
        0, 0, 0);

    width = static_cast<unsigned int>(_width);
    height = static_cast<unsigned int>(_height);

    switch (color_type) {
        case PNG_COLOR_TYPE_RGB:
            color_format = ColorFormatRGB;
            break;

        case PNG_COLOR_TYPE_RGBA:
            color_format = ColorFormatRGBA;
            break;

        default:
            png_destroy_read_struct(&png_ptr, 0, 0);
            fclose(f);
            throw PNGException("Unrecognized PNG color type: " + filename);
            break;
    }

    if (bit_depth != 8) {
        png_destroy_read_struct(&png_ptr, 0, 0);
        fclose(f);
        throw PNGException("Invalid PNG bit depth, must be 8: " + filename);
    }

    /* read data stream */
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    pic = new unsigned char[row_bytes * height];
    png_bytepp rows = png_get_rows(png_ptr, info_ptr);

    unsigned char *dest = pic;
    for (unsigned int i = 0; i < height; i++) {
        memcpy(dest, rows[i], row_bytes);
        dest += row_bytes;
    }

    /* clean up and close file */
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    fclose(f);
}

void PNG::read_png_from_zip(const std::string& filename, ZipReader *zip) throw (PNGException) {
    PNGZipStream zs(zip);

    try {
        zs.data = zs.ptr = zip->extract(filename, &zs.size);
    } catch (const ZipReaderException& e) {
        throw PNGException(e.what());
    }

    try {
        png_bytep pdta = reinterpret_cast<png_bytep>(const_cast<char *>(zs.data));
        png_structp png_ptr;
        png_infop info_ptr;

        /* check header */
        if (png_sig_cmp(pdta, 0, 8)) {
            throw PNGException("File is not recognized as PNG file: " + filename);
        }
        //pdta += 8;

        /* initialize png */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png_ptr) {
            throw PNGException("png_create_read_struct() failed");
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, 0, 0);
            throw PNGException("png_create_info_struct() failed");
        }

        /* setup error handler */
        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, 0, 0);
            throw PNGException("setjmp of png_jmpbuf() failed");
        }

        /* read header informations */
        png_set_read_fn(png_ptr, &zs, user_data_read);
        //png_set_sig_bytes(png_ptr, 8);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

        png_uint_32 _width;
        png_uint_32 _height;
        int color_type;
        png_get_IHDR(png_ptr, info_ptr, &_width, &_height, &bit_depth, &color_type,
            0, 0, 0);

        width = static_cast<unsigned int>(_width);
        height = static_cast<unsigned int>(_height);

        switch (color_type) {
            case PNG_COLOR_TYPE_RGB:
                color_format = ColorFormatRGB;
                break;

            case PNG_COLOR_TYPE_RGBA:
                color_format = ColorFormatRGBA;
                break;

            default:
                png_destroy_read_struct(&png_ptr, 0, 0);
                throw PNGException("Unrecognized PNG color type: " + filename);
                break;
        }

        if (bit_depth != 8) {
            png_destroy_read_struct(&png_ptr, 0, 0);
            throw PNGException("Invalid PNG bit depth, must be 8: " + filename);
        }

        /* read data stream */
        unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
        pic = new unsigned char[row_bytes * height];
        png_bytepp rows = png_get_rows(png_ptr, info_ptr);

        unsigned char *dest = pic;
        for (unsigned int i = 0; i < height; i++) {
            memcpy(dest, rows[i], row_bytes);
            dest += row_bytes;
        }

        /* clean up and close file */
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    } catch (const PNGException& e) {
        if (zs.data) {
            zip->destroy(zs.data);
        }
        throw;
    }

    if (zs.data) {
        zip->destroy(zs.data);
    }
}

void PNG::user_data_read(png_structp png_ptr, png_bytep data, png_size_t len) {
    PNGZipStream *zs = static_cast<PNGZipStream *>(png_get_io_ptr(png_ptr));
    size_t remain = static_cast<size_t>(zs->size) - (zs->ptr - zs->data);
    size_t my_len = static_cast<size_t>(len);
    if (my_len > remain) {
        my_len = remain;
    }
    memcpy(data, zs->ptr, my_len);
    zs->ptr += my_len;
}
