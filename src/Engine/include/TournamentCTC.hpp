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

#ifndef _TOURNAMENTCTC_HPP_
#define _TOURNAMENTCTC_HPP_

#include "Tournament.hpp"

class TournamentCTC : public Tournament {
public:
    TournamentCTC(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentCTC();

    virtual const char *tournament_type();
    virtual std::string get_game_type_name() const;
    virtual void write_stats_in_server_log();
    virtual void update_wearable_remaining(GTimeRemaining *remain);

private:
    struct TimesOfPlayer {
        TimesOfPlayer(Player *p) : player(p), total(0), current(0),
            send_counter(0), send_current(0) { }

        Player *player;
        float total;
        float current;
        int send_counter;
        sr_milliseconds_t send_current;
        gametime_t start_time;

        bool operator <(const TimesOfPlayer& rhs) const {
            return total + current > rhs.total + rhs.current;
        }
    };

    typedef std::vector<TimesOfPlayer> TimesOfPlayers;

    GameObject *coin;
    Icon *enemy_indicator_coin;

    TimesOfPlayers times_of_players;
    char buffer[128];

    virtual void frag_point(Player *pfrag, Player *pkill);
    virtual void subintegrate(ns_t ns);
    virtual void player_added(Player *p);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual bool pick_item(Player *p, GameObject *obj);
    virtual void draw_object_addons();
    virtual void draw_player_addons();
    virtual void draw_enemies_on_hud();
    virtual void score_transport_raw(void *data);
    virtual void draw_statistics();
    virtual void draw_score();
    virtual GenericData *create_generic_data();
    virtual void destroy_generic_data(void *data);
    virtual void generic_data_delivery(void *data);

    void check_coin_validity(double period_f);
    void return_coin(int flags);
    bool coin_not_at_origin();
    void send_coin_remaining();
    void send_coin_timer(Player *p);
    void send_coin_timer(TimesOfPlayer *top);
    bool test_and_drop_coin(Player *p);
    TimesOfPlayer *get_times_of_player(Player *p);
    TimesOfPlayer *get_times_of_player(player_id_t id);
    void update_stats(TimesOfPlayer *top, sr_milliseconds_t total, sr_milliseconds_t current);
    int draw_stats(Font *f, int x, int y);
};

#endif
