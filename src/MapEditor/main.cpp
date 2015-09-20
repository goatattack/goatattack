#include "Globals.hpp"
#include "Resources.hpp"
#include "SubsystemSDL.hpp"
#include "MapEditor.hpp"
#include "Configuration.hpp"

int main(int argc, char *argv[]) {
    try {
        std::ostream& stream = std::cout;

        stream << "welcome to goat attack map editor ";
        stream << GameVersion;
        stream << "...\n" << std::endl;

        init_hpet();

        /* load SDL subsystem */
        SubsystemSDL subsystem(stream, "Goat Attack Map Editor");
        subsystem.set_keep_pictures(true);
        Resources resources(subsystem, STRINGIZE_VALUE_OF(DATA_DIRECTORY));

        /* setup base view options */
        Configuration config(UserDirectory, ConfigFilename);
        if (config.get_bool("fullscreen")) {
            subsystem.toggle_fullscreen();
        }
        subsystem.set_scanlines(config.get_bool("show_scanlines"));
        subsystem.set_scanlines_intensity(config.get_int("scanlines_intensity") / 100.0f);

        /* start map editor */
        MapEditor editor(resources, subsystem, config);
        editor.run();
    } catch (const Exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
