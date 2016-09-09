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

#include "MasterServer.hpp"

#include <cstdlib>

int main(int argc, char *argv[]) {
    const char *filename = "";
    int refresh = 0;
    if (argc > 1 && argv[1]) {
        filename = argv[1];
    }
    if (argc > 2) {
        refresh = atoi(argv[2]);
        if (refresh < 5) {
            refresh = 5;
        } else if (refresh > 30) {
            refresh = 30;
        }
    }
    MasterServer master(25112, 25113, filename, refresh);
    master.run();

    return 0;
}
