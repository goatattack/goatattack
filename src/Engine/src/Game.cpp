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

#include "Game.hpp"

#include "Tournament.hpp"
#include "ScopeCollector.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "MainMenu.hpp"
#include "Globals.hpp"

#include <cstdlib>

Game::Game(Resources& resources, Subsystem& subsystem, Configuration& config)
    : resources(resources), subsystem(subsystem), config(config)
{
    subsystem << "working with " << subsystem.get_subsystem_name() << std::endl;
    subsystem << "mode is " << (subsystem.is_dedicated_server() ?
        "dedicated server" : "normal game") << std::endl;
}

Game::~Game() { }

void Game::run(const std::string& parm) {
    subsystem.initialize(resources);

#ifdef DEDICATED_SERVER
    config.set_save_at_exit(false);
    Server server(resources, subsystem, parm);
    server.start();
#else
    if (config.get_bool("fullscreen")) {
        subsystem.toggle_fullscreen();
    }
    subsystem.set_scanlines(config.get_bool("show_scanlines"));
    subsystem.set_scanlines_intensity(config.get_int("scanlines_intensity") / 100.0f);
    subsystem.set_music_volume(config.get_int("music_volume"));
    subsystem.set_sound_volume(config.get_int("sfx_volume"));
    subsystem.set_deadzone_horizontal(config.get_int("deadzone_horizontal"));
    subsystem.set_deadzone_vertical(config.get_int("deadzone_vertical"));
    MainMenu menu(resources, subsystem, config);
    menu.run();
#endif
}
