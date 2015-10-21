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
        Resources resources(subsystem, STRINGIZE_VALUE_OF(DATA_DIRECTORY));
#endif
        Game game(resources, subsystem);
        game.run(argc > 1 ? argv[1] : "");
    } catch (const Exception& e) {
        stream << e.what() << std::endl;
    }
    stop_net();

    stream << "\nbye bye... :)" << std::endl;

    return 0;
}