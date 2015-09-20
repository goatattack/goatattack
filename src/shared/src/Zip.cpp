#include "Zip.hpp"
#include "SHA256.hpp"
#include "Utils.hpp"

#include <cstring>

Zip::Zip(const std::string& filename) throw (ZipException) : filename(filename), f(0) {
    size_t sz = filename.length();
    size_t pos = 0;
    for (int i = sz - 1; i >= 0; i--) {
        if (filename[i] == dir_separator[0]) {
            pos = i + strlen(dir_separator);
            break;
        }
    }
    this->short_filename.assign(&filename[pos], sz - pos);
}

Zip::~Zip() {
    if (f) {
        fclose(f);
    }
}

const std::string& Zip::get_zip_filename() const {
    return filename;
}

const std::string& Zip::get_zip_short_filename() const {
    return short_filename;
}

const std::string& Zip::get_hash() throw (ZipException) {
    if (!hash.length()) {
        rehash();
    }

    return hash;
}

void Zip::rehash() throw (ZipException) {
    if (!f) {
        throw ZipException("No file opened");
    }

    SHA256 sha256;
    fseek(f, 0, SEEK_SET);
    while (true) {
        size_t sz = fread(buffer, 1, sizeof buffer, f);
        if (sz) {
            sha256.process(buffer, sz);
        }
        if (!sz) {
            break;
        }
    }
    sha256.final();
    hash = sha256.get_hash();
}
