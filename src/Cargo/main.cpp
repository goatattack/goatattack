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

#include "Cargo.hpp"
#include "Globals.hpp"
#include "FileReader.hpp"
#include "CRC64.hpp"
#include "ZipReader.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include "Win.hpp"
#endif

static const int ChunkSize = 1024;

static void create_directory(const char *root, const char *dir) throw (CargoException) {
    const char *separator = dir;
    while (true) {
        /* find separator */
        bool found = false;
        while (separator) {
            separator++;
            if (*separator == 0) {
                break;
            }
            if (*separator == '/' || *separator == '\\') {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }

        /* create directory */
        std::string new_dir(root);
        new_dir += "/";
        new_dir.append(dir, separator - dir);

#ifndef _WIN32
        struct stat st;
        if (stat(new_dir.c_str(), &st) == -1) {
            mkdir(new_dir.c_str(), 0700);
        }
#else
        modify_directory_separator(new_dir);
        wchar_t wdir[MaxPathLength];
        to_unicode(new_dir.c_str(), wdir, MaxPathLength);
        int rv = CreateDirectoryW(wdir, 0);
#endif
    }
}

static void extract(const char *pakfile, const char *root) {
    ZipReader zr(pakfile);
    const Zip::Files& files = zr.get_files();
    for (Zip::Files::const_iterator it = files.begin(); it != files.end(); it++) {
        create_directory(root, it->filename.c_str());
        FILE *f = 0;
        const char *data = 0;
        try {
            std::string new_file(root);
            new_file += "/" + it->filename;
            modify_directory_separator(new_file);
            f = fopen(new_file.c_str(), "wb");
            if (!f) {
                throw CargoException(std::string("Cannot open file for writing: ") + it->filename + " (" + strerror(errno) + ")");
            }
            size_t sz;
            data = zr.extract(it->filename, &sz);
            fwrite(data, 1, sz, f);
            zr.destroy(data);
            fclose(f);
        } catch (const Exception& e) {
            if (f) {
                fclose(f);
            }
            if (data) {
                zr.destroy(data);
            }
            throw;
        }
    }

    std::cout << zr.get_files().size() << " files extracted." << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "Cargo, packager utility for Goat Attack paks (" << GameVersion << ")" << std::endl;

    try {
        switch (argc) {
            case 4:
                if (!strcmp(argv[1], "-p") || !strcmp(argv[1], "--pack")) {
                    Cargo cargo(argv[2], argv[3]);
                    cargo.pack();
                    std::cout << "Successfully " << cargo.packaged() << " files packaged. (hash: " << cargo.get_hash() << ")" << std::endl;
                    return 0;
                } else if (!strcmp(argv[1], "-x") || !strcmp(argv[1], "--extract")) {
                    extract(argv[2], argv[3]);
                    return 0;
                }
                break;

            case 3:
                if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--list")) {
                    ZipReader zr(argv[2]);
                    const Zip::Files& files = zr.get_files();
                    for (Zip::Files::const_iterator it = files.begin(); it != files.end(); it++) {
                        std::cout << it->filename << std::endl;
                    }
                    std::cout << files.size() << " files in package." << std::endl;
                    return 0;
                } else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--hash")) {
                    FileReader fr(argv[2]);
                    CRC64 crc;
                    unsigned char in[ChunkSize];
                    while (!fr.eof()) {
                        size_t sz = fr.read(in, sizeof(in));
                        crc.process(in, sz);
                    }
                    std::cout << "Hash: " << crc.get_hash() << std::endl;
                    return 0;
                } else if (!strcmp(argv[1], "-x") || !strcmp(argv[1], "--extract")) {
                    extract(argv[2], ".");
                    return 0;
                }
                break;
        }
    } catch (const Exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Usage " << argv[0] << " OPTION PARAMETERS" << std::endl;
    std::cout << "  -p, --pack      pack, path" << dir_separator << "to";
    std::cout << dir_separator << "files outfile.pak" << std::endl;
    std::cout << "  -x, --extract   extract, pakfile.pak [destination]" << std::endl;
    std::cout << "  -l, --list      list, pakfile.pak" << std::endl;
    std::cout << "  -h, --hash      hash, pakfile.pak" << std::endl;
    std::cout << std::endl;
    return 2;
}
