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

#ifdef DEDICATED_SERVER
    #include "SubsystemNull.hpp"
#else
    #include "SubsystemSDL.hpp"
#endif

#include "Utils.hpp"
#include "Game.hpp"
#include "Globals.hpp"
#include "Timing.hpp"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <cstdio>

int main(int argc, char *argv[]) {
    std::ostream& stream = std::cout;
    const char *parm = (argc > 1 ? argv[1] : 0);

    stream << "welcome to goat attack ";
    stream << GameVersion;
    stream << "...\n" << std::endl;

    init_hpet();
    start_net();
    try {
#ifdef DEDICATED_SERVER
        SubsystemNull subsystem(stream, "Goat Attack");
#else
        SubsystemSDL subsystem(stream, "Goat Attack");
#endif

#ifdef __APPLE__
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        char path[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
        {
            throw Exception("Cannot get bundle path");
        }
        CFRelease(resourcesURL);
        std::string data_directory(path);
        data_directory += "/data";
        Resources resources(subsystem, data_directory);
#else
# ifdef DEDICATED_SERVER
        const char *data_directory = STRINGIZE_VALUE_OF(DATA_DIRECTORY);
# else
        const char *data_directory = (parm ? parm : STRINGIZE_VALUE_OF(DATA_DIRECTORY));
# endif
        Resources resources(subsystem, data_directory);
#endif
        Game game(resources, subsystem);
        game.run(parm ? parm : "");
    } catch (const Exception& e) {
        stream << e.what() << std::endl;
    }
    stop_net();

    stream << "\nbye bye... :)" << std::endl;

    return 0;
}