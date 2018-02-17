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

#ifndef _TOURNAMENTSR_HPP_
#define _TOURNAMENTSR_HPP_

#include "Tournament.hpp"

class TournamentSR : public Tournament {
public:
    TournamentSR(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup);
    virtual ~TournamentSR();

    virtual const char *tournament_type();
    virtual std::string get_game_type_name() const;
    virtual void write_stats_in_server_log();

private:
    typedef std::vector<float> Times;
    typedef std::vector<Player *> PlayersToReturn;

    struct TimesOfPlayer {
        TimesOfPlayer(Player *player)
            : player(player), best(-1.0f), last(-1.0f),
              running(false), finished(false)
        {
            get_now(start_time);
        }

        Player *player;
        float best;
        float last;
        bool running;
        bool finished;

        gametime_t start_time;
        Times times;

        bool operator <(const TimesOfPlayer& rhs) const {
            if (times.size() > 0) {
                if (rhs.times.size() > 0) {
                    return (best < rhs.best);
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }
    };

    typedef std::vector<TimesOfPlayer> TimesOfPlayers;

    TimesOfPlayers times_of_players;
    PlayersToReturn players_to_return;
    int max_rounds;
    char buffer[128];
    gametime_t now_for_drawing;

    virtual void frag_point(Player *pfrag, Player *pkill);
    virtual bool tile_collision(TestType type, Player *p, int last_falling_y_pos,
        Tile *t, bool *killing);

    virtual void player_added(Player *p);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual void players_post_actions();
    virtual void spawn_player(Player *p);
    virtual void round_finished_set_time(Player *p, GTransportTime *race);
    virtual void draw_statistics();
    virtual void score_transport_raw(void *data);
    virtual void draw_score();
    virtual void reset_player(Player *p);
    virtual GenericData *create_generic_data();
    virtual void destroy_generic_data(void *data);
    virtual void generic_data_delivery(void *data);

    int draw_stats(Font *f, int x, int y);
    TimesOfPlayer *get_times_of_player(Player *p);
    TimesOfPlayer *get_times_of_player(player_id_t id);
    void update_stats(TimesOfPlayer *top, float t);
};

#endif
