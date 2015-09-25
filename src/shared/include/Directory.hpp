#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include "Exception.hpp"
#include "ZipReader.hpp"

#include <string>
#ifdef __unix__
#include <dirent.h>
#elif _WIN32
#include "Win.hpp"
#endif

class DirectoryException : public Exception {
public:
    DirectoryException(const char *msg) : Exception(msg) { }
    DirectoryException(const std::string& msg) : Exception(msg) { }
};

class Directory {
private:
    Directory(const Directory&);
    Directory& operator=(const Directory&);

public:
    Directory(const std::string& directory, const std::string& suffix, ZipReader *zip = 0) throw (DirectoryException);
    virtual ~Directory();

    const char *get_entry();

private:
    std::string directory;
    std::string suffix;
    bool finished;
    ZipReader *zip;
    Zip::Files::const_iterator zf_it;
    char zip_ret[256];
#ifdef __unix__
    DIR *dir;
#elif _WIN32
    HANDLE dir;
    WIN32_FIND_DATAA ffd;
#endif
};

#endif
