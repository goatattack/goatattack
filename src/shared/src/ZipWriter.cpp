#include "ZipWriter.hpp"

ZipWriter::ZipWriter(const std::string& filename) throw (ZipWriterException)
    : Zip(filename)
{ 
    throw ZipWriterException("not implemented yet."); 
}

ZipWriter::~ZipWriter() { }

void ZipWriter::add_file(const std::string& filename) throw (ZipWriterException) {
}

void ZipWriter::remove_file(const std::string& filename) throw (ZipWriterException) {
}
