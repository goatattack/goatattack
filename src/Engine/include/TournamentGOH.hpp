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

#ifndef _TOURNAMENTGOH_HPP_
#define _TOURNAMENTGOH_HPP_

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * see TournamentTeam
 *
 */

class TournamentGOH : public TournamentTeam {
public:
    TournamentGOH(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentGOH();

    virtual const char *tournament_type();
    virtual void team_fire_point(Player *penem, Team team, int points);
    virtual void score_transport_raw(void *data);

private:
    Player *first_player_on_hill;
    Player *addon_player;
    double hill_counter;
    std::string crested_sound;
    std::string scored_sound;
    std::string leaved_sound;

    GHillCounter draw_hill_counter;

    virtual void subintegrate(ns_t ns);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual void draw_player_addons();
    virtual GenericData *create_generic_data();
    virtual void destroy_generic_data(void *data);
    virtual void generic_data_delivery(void *data);

    bool player_is_in_hill_zone(Player *p);
    void test_and_remove_player_from_hill(Player *p);
    void send_hill_counter(pos_t value);
    GHillCounter *create_hill_counter(pos_t value);
};

#endif
