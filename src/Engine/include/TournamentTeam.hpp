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

#ifndef _TOURNAMENTTEAM_HPP_
#define _TOURNAMENTTEAM_HPP_

#include "Tournament.hpp"
#include "ButtonNavigator.hpp"

/* pointing system:
 *
 * kill             1
 * team kill       -2
 * see Tournament
 *
 */

class TournamentTeam : public Tournament {
public:
    TournamentTeam(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentTeam();

protected:
    enum Team {
        TeamRed = 0,
        TeamBlue
    };

    Icon *team_badge_red;
    Icon *team_badge_blue;
    Icon *enemy_indicator_red;
    Icon *enemy_indicator_blue;
    Icon *enemy_indicator_red_flag;
    Icon *enemy_indicator_blue_flag;
    double check_unbalancing;
    bool choose_team_open;
    ButtonNavigator *nav;

    GTeamScore score;
    SpawnPoints spawn_points_red;
    SpawnPoints spawn_points_blue;

    virtual void subintegrate(ns_t ns);
    virtual void create_spawn_points();
    virtual void draw_team_colours();
    virtual void draw_statistics();
    virtual void spawn_player(Player *p);
    virtual void retrieve_states();
    virtual void add_team_score(GTeamScore *ts);
    virtual void frag_point(Player *pfrag, Player *pkill);
    virtual void player_join_request(Player *p);
    virtual bool player_joins(Player *p, playerflags_t flags);
    virtual void check_friendly_fire(identifier_t owner, Player *p);
    virtual bool friendly_fire_alarm(GFriendyFireAlarm *alarm);
    virtual void draw_enemies_on_hud();
    virtual void reopen_join_window(Player *p);

    void send_team_score();
    int draw_team_stats(Font* f, int x, int y, playerflags_t flags);
    void draw_team_score(int x, int y, bool rightalign, score_t score);
    void add_team_score_animation(Player *p, I18NText id);

    static void static_red_team_click(GuiVirtualButton *sender, void *data);
    static void static_blue_team_click(GuiVirtualButton *sender, void *data);
    void team_click(playerflags_t flags);
    void close_select_team_window();

    virtual void write_stats_in_server_log();

    virtual void team_fire_point(Player *penem, Team team, int points) = 0;
};

#endif
