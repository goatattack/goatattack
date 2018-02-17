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

#ifndef _TOURNAMENTDM_HPP_
#define _TOURNAMENTDM_HPP_

#include "Tournament.hpp"

/* pointing system:
 *
 * frag             1
 * see Tournament
 *
 */

class TournamentDM : public Tournament {
public:
    TournamentDM(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentDM();

    virtual const char *tournament_type();
    virtual std::string get_game_type_name() const;
    virtual void write_stats_in_server_log();

    virtual void frag_point(Player *pfrag, Player *pkill);
    virtual void draw_statistics();

private:
    int draw_stats(Font *f, int x, int y);
};

#endif
