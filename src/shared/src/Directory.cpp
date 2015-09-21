#include "Directory.hpp"

#include <cstring>
#include <cerrno>

Directory::Directory(const std::string& directory, const std::string& suffix,
    ZipReader *zip) throw (DirectoryException)
    : directory(directory), suffix(suffix), finished(false), zip(zip)
{
    if (zip) {
        const Zip::Files& files = zip->get_files();
        zf_it = files.begin();
    } else {
#ifdef __unix__
        dir = opendir(directory.c_str());
        if (!dir) {
#elif _WIN32
        std::string wildcard = directory + "\\*.*";
        dir = FindFirstFileA(wildcard.c_str(), &ffd);
        if (dir == INVALID_HANDLE_VALUE) {
#endif
            throw DirectoryException("Cannot look into directory " + directory + ": "
                + std::string(strerror(errno)));
        }
    }
}

Directory::~Directory() {
    if (!zip) {
#ifdef __unix__
        closedir(dir);
#elif _WIN32
        FindClose(dir);
#endif
    }
}

const char *Directory::get_entry() {
    if (zip) {
        const Zip::Files& files = zip->get_files();
        while (zf_it != files.end()) {
            const Zip::File& file = *zf_it;
            zf_it++;
            memset(zip_ret, 0, sizeof zip_ret);
            if (zip->equals_directory(file, directory)) {
                if (suffix.length()) {
                    /* find suffix */
                    size_t pos = file.filename.rfind('.');
                    if (pos != std::string::npos) {
                        if (!strcmp(&file.filename.c_str()[pos], suffix.c_str())) {
                            if (pos >= sizeof zip_ret) {
                                pos = sizeof zip_ret - 1;
                            }
                            strncpy(zip_ret, file.filename.c_str(), pos);
                            return zip_ret;
                        }
                    }
                } else {
                    strncpy(zip_ret, file.filename.c_str(), sizeof zip_ret - 1);
                    return zip_ret;
                }
            }
        }
    } else {
        if (!finished) {
#ifdef __unix__
            struct dirent *entry;
            while ((entry = readdir(dir))) {
                if (!entry) {
                    finished = true;
                    break;
                } else {
                    if (suffix.length()) {
                        /* find suffix */
                        char *p = entry->d_name;
                        while (*p) p++;
                        while (p != entry->d_name && *p != '.') {
                            p--;
                        }
                        if (!strcmp(p, suffix.c_str())) {
                            *p = 0;
                            return entry->d_name;
                        }
                    } else {
                        return entry->d_name;
                    }
                }
            }
#elif _WIN32
            do {
                if (suffix.length()) {
                    char *p = ffd.cFileName;
                    while (*p) p++;
                    while (p != ffd.cFileName && *p != '.') {
                        p--;
                    }
                    if (!strcmp(p, suffix.c_str())) {
                        *p = 0;
                        return ffd.cFileName;
                    }

                } else {
                    return static_cast<char *>(ffd.cFileName);
                }
            } while (FindNextFileA(dir, &ffd));
#endif
        }
    }

    return 0;
}
