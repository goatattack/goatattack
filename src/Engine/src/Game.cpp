#include "Game.hpp"

#include "Tournament.hpp"
#include "ScopeCollector.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "MainMenu.hpp"
#include "Globals.hpp"

#include <cstdlib>

Game::Game(Resources& resources, Subsystem& subsystem)
    : resources(resources), subsystem(subsystem),
      config(UserDirectory, ConfigFilename)
{
    subsystem << "working with " << subsystem.get_subsystem_name() << std::endl;
    subsystem << "mode is " << (subsystem.is_dedicated_server() ?
        "dedicated server" : "normal game") << std::endl;
}

Game::~Game() { }

void Game::run(const std::string& parm) {
#ifdef DEDICATED_SERVER
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
