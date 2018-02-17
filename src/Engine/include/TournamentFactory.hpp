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

#ifndef _TOURNAMENTFACTORY_HPP_
#define _TOURNAMENTFACTORY_HPP_

#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Player.hpp"
#include "Gui.hpp"
#include "ServerLogger.hpp"
#include "Configuration.hpp"
#include "MapConfiguration.hpp"

#include <string>

class TournamentFactoryException : public Exception {
public:
    TournamentFactoryException(const char *msg) : Exception(msg) { }
    TournamentFactoryException(const std::string& msg) : Exception(msg) { }
};

class TournamentFactory {
private:
    TournamentFactory(const TournamentFactory&);
    TournamentFactory& operator=(const TournamentFactory&);

public:
    TournamentFactory(Resources& resources, Subsystem& subsystem, Gui *gui);
    virtual ~TournamentFactory();

    Tournament *create_tournament(const MapConfiguration& config,
        bool server, bool warmup, Players& players, ServerLogger *logger);

    unsigned char get_tournament_id() const;
    void set_tournament_id(unsigned char id);
    void set_tournament_server_flags(Properties& properties, Tournament *tournament);
    void set_server_name(Tournament *tournament, const std::string& name);

private:
    Resources& resources;
    Subsystem& subsystem;
    Gui *gui;
    unsigned char tournament_id;
    std::string server_name;
};

#endif
