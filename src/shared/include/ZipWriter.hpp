#ifndef ZIPWRITER_HPP
#define ZIPWRITER_HPP

#include "Zip.hpp"

#include <string>
#include <vector>
#include <cstdio>

class ZipWriterException : public ZipException {
public:
    ZipWriterException(const char *msg) : ZipException(msg) { }
    ZipWriterException(const std::string& msg) : ZipException(msg) { }
};

class ZipWriter : public Zip {
private:
    ZipWriter(const ZipWriter&);
    ZipWriter& operator=(const ZipWriter&);

public:
    ZipWriter(const std::string& filename) throw (ZipWriterException);
    virtual ~ZipWriter();

    void add_file(const std::string& filename) throw (ZipWriterException);
    void remove_file(const std::string& filename) throw (ZipWriterException);
};

#endif
