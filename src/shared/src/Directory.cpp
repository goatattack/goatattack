/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Directory.hpp"
#include "Utils.hpp"

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
        bool add_separator = true;
        if (directory.length()) {
            add_separator = (directory[directory.length() - 1] != '\\');
        }
        std::string wildcard = directory + (add_separator ? "\\" : "") + "*.*";
        wchar_t buffer[MaxPathLength];
        to_unicode(wildcard.c_str(), buffer, MaxPathLength);
        dir = FindFirstFileW(buffer, &ffd);
        first = true;
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
            while (true) {
                if (!first) {
                    if (!FindNextFileW(dir, &ffd)) {
                        break;
                    }
                } else {
                    first = false;
                }
                from_unicode(ffd.cFileName, directory_ret, sizeof(directory_ret));
                if (suffix.length()) {
                    char *p = directory_ret;
                    while (*p) p++;
                    while (p != directory_ret && *p != '.') {
                        p--;
                    }
                    if (!strcmp(p, suffix.c_str())) {
                        *p = 0;
                        return directory_ret;
                    }

                } else {
                    return directory_ret;
                }
            }
#endif
        }
    }

    return 0;
}
