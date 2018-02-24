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

#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Resources.hpp"
#include "Configuration.hpp"

class GameException : public Exception {
public:
    GameException(const char *msg) : Exception(msg) { }
    GameException(std::string msg) : Exception(msg) { }
};

class Game {
private:
    Game(const Game&);
    Game& operator=(const Game&);

public:
    Game(Resources& resources, Subsystem& subsystem, Configuration& config);

    void run(const std::string& parm);

private:
    Resources& resources;
    Subsystem& subsystem;
    Configuration& config;
};

#endif
