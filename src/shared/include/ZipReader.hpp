#ifndef ZIPREADER_HPP
#define ZIPREADER_HPP

#include "Zip.hpp"

#include <string>
#include <vector>
#include <cstdio>

class ZipReaderException : public ZipException {
public:
    ZipReaderException(const char *msg) : ZipException(msg) { }
    ZipReaderException(const std::string& msg) : ZipException(msg) { }
};

class ZipReader : public Zip {
private:
    ZipReader(const ZipReader&);
    ZipReader& operator=(const ZipReader&);

public:
    ZipReader(const std::string& filename) throw (ZipReaderException);
    virtual ~ZipReader();

    const Files& get_files() const;
    bool file_exists(std::string filename);
    bool equals_directory(const File& file, const std::string& directory);
    const char *extract(std::string filename, size_t *out_sz = 0) throw (ZipReaderException);
    static void destroy(const char *data);

private:
    void throw_corrupt_file(const std::string& filename) throw (ZipReaderException);
    void throw_inflate_failed(z_stream *z, const char *data, const std::string& msg) throw (ZipReaderException);
    const File& get_file(const std::string& filename) throw (ZipReaderException);
};

#endif
