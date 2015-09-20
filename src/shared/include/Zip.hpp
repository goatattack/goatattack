#ifndef ZIP_HPP
#define ZIP_HPP

#include "Exception.hpp"

#include "zlib.h"

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

    Zip(const std::string& filename) throw (ZipException);
    virtual ~Zip();

    const std::string& get_zip_filename() const;
    const std::string& get_zip_short_filename() const;
    const std::string& get_hash() throw (ZipException);
    void rehash() throw (ZipException);

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
