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
        Resources resources(subsystem, STRINGIZE_VALUE_OF(DATA_DIRECTORY), true);

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
