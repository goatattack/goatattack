#include "ZipReader.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>

ZipReader::ZipReader(const std::string& filename) throw (ZipReaderException)
    : Zip(filename)
{
    size_t sz;
    unsigned char *ptr;

    /* open file */
    f = fopen(filename.c_str(), "rb");
    if (!f) {
        throw ZipReaderException("Can't open package: " + filename + " (" + strerror(errno) + ")");
    }

    /* read magic */
    sz = fread(buffer, 1, 4, f);
    if (sz != 4 || memcmp(buffer, "PK\003\004", 4)) {
        throw_corrupt_file(filename);
    }

    /* find end of central directory */
    fseek(f, -256, SEEK_END);
    sz = fread(buffer, 1, 256, f);
    if (sz < 22) {
        throw_corrupt_file(filename);
    }

    ptr = buffer + sz - 22;
    bool ok = false;
    while (ptr >= buffer) {
        if (!memcmp(ptr, "PK\005\006", 4)) {
            ok = true;
            break;
        }
        ptr--;
    }
    if (!ok) {
        throw_corrupt_file(filename);
    }
    int c_d_entries = ptr[11] << 8 | ptr[10];
    int c_d_pos = ptr[19] << 24 | ptr[18] << 16 | ptr[17] << 8 | ptr[16];

    /* read out central directory */
    fseek(f, c_d_pos, SEEK_SET);
    for (int i = 0; i < c_d_entries; i++) {
        sz = fread(buffer, 1, 46, f);
        if (sz != 46) {
            throw_corrupt_file(filename);
        }
        if (memcmp(buffer, "PK\001\002", 4)) {
            throw_corrupt_file(filename);
        }
        File entry;
        int len = buffer[29] << 8 | buffer[28];
        int xln = (buffer[31] << 8 | buffer[30]);
        int cmt = (buffer[33] << 8 | buffer[32]);
        entry.sz = buffer[19] << 8 | buffer[18];
        entry.ofs = buffer[45] << 24 | buffer[44] << 16 | buffer[43] << 8 | buffer[42];
        sz = fread(buffer, 1, len + xln + cmt , f);
        if (sz != static_cast<size_t>(len + xln + cmt)) {
            throw_corrupt_file(filename);
        }
        entry.filename.assign(reinterpret_cast<const char *>(buffer), len);
        files.push_back(entry);
    }
}

ZipReader::~ZipReader() { }

const ZipReader::Files& ZipReader::get_files() const {
    return files;
}

bool ZipReader::file_exists(std::string filename) {
    try {
        std::replace(filename.begin(), filename.end(), '\\', '/');
        get_file(filename);
        return true;
    } catch (const ZipReaderException&) {
        /* chomp */
    }

    return false;
}
bool ZipReader::equals_directory(const File& file, const std::string& directory) {
    size_t sz = directory.length() + 1;
    if (file.filename.length() >= sz) {
        if (!memcmp(directory.c_str(), file.filename.c_str(), sz - 1)) {
            if (file.filename.c_str()[sz - 1] == '/') {
                return true;
            }
        }
    }

    return false;
}

const char *ZipReader::extract(std::string filename, size_t *out_sz) throw (ZipReaderException) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    const File& file = get_file(filename);

    /* get file header */
    fseek(f, file.ofs, SEEK_SET);
    size_t sz = fread(buffer, 1, 30, f);
    if (sz != 30) {
        throw_inflate_failed(0, 0, filename);
    }
    if (memcmp(buffer, "PK\003\004", 4)) {
        throw_inflate_failed(0, 0, filename);
    }
    int cmpr_sz = buffer[21] << 24 | buffer[20] << 16 | buffer[19] << 8 | buffer[18];
    int ucmpr_sz = buffer[25] << 24 | buffer[24] << 16 | buffer[23] << 8 | buffer[22];
    int len = buffer[27] << 8 | buffer[26];
    int xln = buffer[29] << 8 | buffer[28];

    /* extract */
    char *data = new char[ucmpr_sz];
    fseek(f, file.ofs + 30 + len + xln, SEEK_SET);
    if (cmpr_sz == ucmpr_sz) {
        /* plain copy */
        sz = fread(data, 1, cmpr_sz, f);
        if (sz != static_cast<size_t>(cmpr_sz)) {
            throw_inflate_failed(0, data, filename);
        }
    } else {
        /* uncompress */
        z_stream z;
        memset(&z, 0, sizeof(z_stream));
        char *dst = data;
        int status = inflateInit2(&z, -MAX_WBITS);
        if (status != Z_OK) {
            throw_inflate_failed(&z, data, filename);
        }

        const int bsz = sizeof buffer;
        do {
            z.next_in = buffer;
            if (cmpr_sz > bsz) {
                z.avail_in = bsz;
            } else {
                z.avail_in = cmpr_sz;
            }
            z.avail_in = fread(z.next_in, 1, z.avail_in, f);
            while (z.avail_in && status == Z_OK) {
                z.next_out = outbuf;
                z.avail_out = bsz;
                status = inflate(&z, Z_NO_FLUSH);
                int c = bsz - z.avail_out;
                if (c) {
                    memcpy(dst, outbuf, c);
                    dst += c;
                }
            }
            cmpr_sz -= bsz;
        } while (cmpr_sz > 0 && status == Z_OK);
        if (status != Z_STREAM_END) {
            throw_inflate_failed(&z, data, filename + " (" + z.msg + ")");
        }
        inflateEnd(&z);
    }

    if (out_sz) {
        *out_sz = static_cast<size_t>(ucmpr_sz);
    }

    return data;
}

void ZipReader::destroy(const char *data) {
    if (data) {
        delete[] data;
    }
}

void ZipReader::throw_corrupt_file(const std::string& filename) throw (ZipReaderException) {
    throw ZipReaderException("Corrupt package file: " + filename);
}

void ZipReader::throw_inflate_failed(z_stream *z, const char *data, const std::string& filename) throw (ZipReaderException) {
    if (z) {
        inflateEnd(z);
    }
    if (data) {
        destroy(data);
    }
    throw ZipReaderException("Inflate failed: " + filename);
}

const ZipReader::File& ZipReader::get_file(const std::string& filename) throw (ZipReaderException) {
    for (Files::iterator it = files.begin(); it != files.end(); it++) {
        const File& file = *it;
        if (file.filename == filename) {
            return file;
        }
    }

    throw ZipReaderException("File " + filename + " not found in package");
}
