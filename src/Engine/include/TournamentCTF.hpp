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

#ifndef _TOURNAMENTCTF_HPP_
#define _TOURNAMENTCTF_HPP_

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * capture flag     3
 * neutralize flag  2
 * and see TournamentTeam
 *
 */

class TournamentCTF : public TournamentTeam {
public:
    TournamentCTF(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentCTF();

    virtual const char *tournament_type();
    virtual std::string get_game_type_name() const;
    virtual void update_wearable_remaining(GTimeRemaining *remain);

private:
    GameObject *red_flag;
    GameObject *blue_flag;

    virtual void subintegrate(ns_t ns);
    virtual void draw_object_addons();
    virtual void draw_player_addons();
    virtual bool pick_item(Player *p, GameObject *obj);

    virtual bool tile_collision(TestType type, Player *p, int last_falling_y_pos,
        Tile *t, bool *killing);

    virtual bool fire_enabled(Player *p);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual bool play_gun_error(Player *p);

    bool test_and_drop_flag(Player *p);
    void return_flag(GameObject *flag, int flags);
    void check_flag_validity(double period_f, I18NText id, GameObject *flag);
    bool flag_not_at_origin(GameObject *flag);
    void send_flag_remaining(GameObject *flag);

    virtual void team_fire_point(Player *penem, Team team, int points);
};

#endif
