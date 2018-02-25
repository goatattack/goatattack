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
#include "I18N.hpp"
#include "PathManager.hpp"

#include <cstdlib>

int main(int argc, char *argv[]) {
    std::ostream& stream = std::cout;
    const char *parm = (argc > 1 ? argv[1] : 0);

    stream << "Goat Attack Map Editor " << GameVersion << std::endl;

    try {
        init_hpet();
        std::srand(static_cast<unsigned int>(time(0)));
        PathManager pm(ApplicationName);
        Configuration config(pm, ConfigFilename);
        I18N i18n(stream, static_cast<I18N::Language>(config.get_int("language")));

        /* load SDL subsystem */
        SubsystemSDL subsystem(stream, i18n, i18n(I18N_WINTITLE_EDITOR), config.get_bool("shading_pipeline"));
        subsystem.set_keep_pictures(true);
        const char *data_directory = (parm ? parm : STRINGIZE_VALUE_OF(DATA_DIRECTORY));
        Resources resources(subsystem, pm, data_directory, true, false);

        /* setup base view options */
        subsystem.initialize(resources);
        if (config.get_bool("fullscreen")) {
            subsystem.toggle_fullscreen();
        }
        subsystem.set_scanlines(config.get_bool("show_scanlines"));
        subsystem.set_scanlines_intensity(config.get_int("scanlines_intensity") / 100.0f);

        /* start map editor */
        MapEditor editor(resources, subsystem, config);
        editor.run();
    } catch (const Exception& e) {
        stream << e.what() << std::endl;
        stream << "Ensure that you can add a data folder as parameter." << std::endl;
        stream << "Example: " << argv[0] << " path/to/your/data/folder" << std::endl;
    }

    stream << "\nbye bye... :)" << std::endl;

    return 0;
}
