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

#ifndef _TOURNAMENTTDM_HPP_
#define _TOURNAMENTTDM_HPP_

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * see TournamentTeam
 *
 */

class TournamentTDM : public TournamentTeam {
public:
    TournamentTDM(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentTDM();

    virtual const char *tournament_type();
    virtual std::string get_game_type_name() const;
    virtual void team_fire_point(Player *penem, Team team, int points);
};

#endif
