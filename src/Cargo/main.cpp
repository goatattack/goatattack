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

#include <iostream>
#include <cstring>

static const int ChunkSize = 1024;

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
                    std::cout << "Not implemented yet." << std::endl;
                    return 1;
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
                }
                break;
        }
    } catch (const Exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Usage " << argv[0] << " OPTION PARAMETERS" << std::endl;
    std::cout << "  -p, --pack      pack, path/to/files outfile.pak" << std::endl;
    std::cout << "  -x, --extract   extract, pakfile.pak destination" << std::endl;
    std::cout << "  -l, --list      list, pakfile.pak" << std::endl;
    std::cout << "  -h, --hash      hash, pakfile.pak" << std::endl;
    std::cout << std::endl;
    return 2;
}