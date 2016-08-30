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

#include "TournamentSR.hpp"

#include <algorithm>
#include <limits>
#include <cmath>
#ifdef _WIN32
#include "Win.hpp"
#endif

TournamentSR::TournamentSR(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : Tournament(resources, subsystem, gui, logger, game_file, server, map_name,
      players, duration, warmup)
{
    /* create spawn points */
    create_spawn_points();
    if (spawn_points.size() != 1) {
        throw TournamentException("Why more than one spawn point in this game mode?");
    }

    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_sr");

    /* add all players into list */
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        times_of_players.push_back(TimesOfPlayer(p));
    }
}

TournamentSR::~TournamentSR() { }

const char *TournamentSR::tournament_type() {
    return "SR";
}

void TournamentSR::write_stats_in_server_log() {
    int rank = 0;
    float last_best_time = 0.0f;
    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = (*it);

        if (!(top.player->state.server_state.flags & PlayerServerFlagSpectating)) {
            bool has_entries = top.times.size() > 0;
            if (rank) {
                if (has_entries) {
                    if (top.best > last_best_time) {
                        rank++;
                    }
                } else {
                    rank++;
                }
            } else {
                rank++;
            }

            int laps = static_cast<int>(top.times.size());
            if (has_entries) {
                logger->log(ServerLogger::LogTypeStatsSR, "player stat", top.player, 0,
                    &rank, &laps, &top.best, &top.last);
                last_best_time = top.best;
            } else {
                logger->log(ServerLogger::LogTypeStatsSR, "player stat", top.player, 0,
                    &rank, &laps, 0, 0);
                last_best_time = 0.0f;
            }
            sprintf(buffer, "%d", rank);
        }
    }
}

void TournamentSR::frag_point(Player *pfrag, Player *pkill) { }

bool TournamentSR::tile_collision(TestType type, Player *p, int last_falling_y_pos,
    Tile *t, bool *killing)
{
    if (killing) {
        *killing = false;
    }

    if (p) {
        Player *me = get_me();
        switch (t->get_tile_type()) {
            case Tile::TileTypeSpeedraceFinish:
                if (!server && p == me) {
                    bool found = false;
                    for (PlayersToReturn::iterator it = players_to_return.begin();
                        it != players_to_return.end(); it++)
                    {
                        if (*it == p) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        players_to_return.push_back(p);
                        TimesOfPlayer *top = get_times_of_player(p);
                        if (top) {
                            top->finished = true;
                        }
                    }
                }
                return false;
                break;

            default:
                break;
        }
    }

    return Tournament::tile_collision(type, p, last_falling_y_pos, t, killing);
}

void TournamentSR::player_added(Player *p) {
    times_of_players.push_back(TimesOfPlayer(p));
}

void TournamentSR::player_removed(Player *p) {
    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        if ((*it).player == p) {
            times_of_players.erase(it);
            break;
        }
    }
}

void TournamentSR::player_died(Player *p) {
    TimesOfPlayer *top = get_times_of_player(p);
    if (top) {
        top->running = false;
    }
}
void TournamentSR::players_post_actions() {
    /* client side */
    if (players_to_return.size()) {
        if (!server) {
            for (PlayersToReturn::iterator it = players_to_return.begin();
                it != players_to_return.end(); it++)
            {
                Player *p = *it;
                spawn_player(p);
            }
        }
        players_to_return.clear();
    }
}

void TournamentSR::spawn_player(Player *p) {
    if (!server) {
        TimesOfPlayer *top = get_times_of_player(p);
        if (top) {
            if (!top->running) {
                top->running = true;
            } else {
                if (top->finished) {
                    top->finished = false;
                    gametime_t now;
                    get_now(now);
                    ms_t d_ms = diff_ms(top->start_time, now);
                    GTransportTime *race = new GTransportTime;
                    race->id = p->state.id;
                    race->ms = static_cast<sr_milliseconds_t>(d_ms);
                    race->to_net();
                    add_state_response(GPSRoundFinished, GTransportTimeLen, race);
                }
            }
            get_now(top->start_time);
        }
    }
    Tournament::spawn_player(p);
}

void TournamentSR::round_finished_set_time(Player *p, GTransportTime *race) {
    /* server side */
    TimesOfPlayer *top = get_times_of_player(p);
    if (top) {
        GTransportTime *tm = new GTransportTime;
        *tm = *race;
        tm->to_net();
        add_state_response(GPCScoreTransportRaw, GTransportTimeLen, tm);
        float diff = race->ms / 1000.0f;

        TimesOfPlayer& first_top = times_of_players[0];
        bool has_entries = first_top.times.size() > 0;
        float best_time = first_top.best;

        update_stats(top, diff);

        if ((has_entries && diff < best_time) || !has_entries) {
            sprintf(buffer, "new record of %s: %.2f", p->get_player_name().c_str(), diff);
            add_msg_response(buffer);
            add_sound_response("round_complete_best");
            if (logger) {
                logger->log(ServerLogger::LogTypeRoundFinished, buffer, p, 0, &diff);
            }
        } else {
            add_sound_response("round_complete");
            if (logger) {
                logger->log(ServerLogger::LogTypeRoundFinished, "round completed", p, 0, &diff);
            }
        }
    }
}

void TournamentSR::draw_statistics() {
    Font *font_normal = resources.get_font("normal");
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = ((vw - 20) / 2) + 40;
    int wh = vh - 20;
    int x = vw / 2 - ww / 2;
    int y = 10;
    int wx = x;

    /* set alpha */
    float alpha = 0.8f;

    /* draw shadow */
    subsystem.set_color(0.0f, 0.0f, 0.0f, 0.2f);
    subsystem.draw_box(x + 7, y + 7, ww, wh);

    /* draw window */
    subsystem.set_color(0.75f, 0.5f, 0.5f, alpha);
    subsystem.draw_box(x, y, ww, wh);

    subsystem.set_color(0.25f, 0.0f, 0.0f, alpha);
    subsystem.draw_box(x + 1, y + 1, ww - 2, wh - 2);

    /* draw black bars */
    subsystem.set_color(0.0f, 0.0f, 0.0f, alpha);
    subsystem.draw_box(x + 10, y + 15, ww - 20, 21);

    /* reset */
    subsystem.reset_color();

    /* draw screws */
    subsystem.draw_icon(screw1, wx + 5, 15);
    subsystem.draw_icon(screw2, wx + ww - 5 - 8, 15);
    subsystem.draw_icon(screw3, wx + 5, vh - 15 - 8);
    subsystem.draw_icon(screw4, wx + ww - 5 - 8, vh - 15 - 8);

    /* draw title */
    Font *font_big = resources.get_font("big");
    std::string txt("SPEED RACE");
    int tw = font_big->get_text_width(txt);
    subsystem.draw_text(font_big, vw / 2 - tw / 2, y + 18, txt);

    /* draw list */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    y = 55;
    x = wx + 15;
    subsystem.draw_text(font_normal, x, y, "#");
    subsystem.draw_text(font_normal, x + 20, y, "PLAYER");
    subsystem.draw_text(font_normal, x + 140, y, "LAPS");
    subsystem.draw_text(font_normal, x + 180, y, "BEST");
    subsystem.draw_text(font_normal, x + 240, y, "LAST");
    subsystem.draw_text(font_normal, x + 300, y, "PING");
    subsystem.reset_color();

    y = draw_stats(font_normal, wx + 15, y + 15);

    /* spectators */
    x = wx + 15;
    y += 50;

    /* done */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_text(font_normal, wx + 15, y, "SPECTACTORS:");
    subsystem.reset_color();
    y += font_normal->get_font_height();
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->state.server_state.flags & PlayerServerFlagSpectating) {
            int w = font_normal->get_text_width(p->get_player_name());
            if (x + w >= wx + ww - 15) {
                x = wx + 15;
                y += font_normal->get_font_height();
            }
            subsystem.draw_text(font_normal, x, y, p->get_player_name());
            x += w + 10;
        }
    }
}

void TournamentSR::score_transport_raw(void *data) {
    GTransportTime *tm = reinterpret_cast<GTransportTime *>(data);
    tm->from_net();
    TimesOfPlayer *top = get_times_of_player(tm->id);
    if (top) {
        update_stats(top, tm->ms / 1000.0f);
    }
}

void TournamentSR::draw_score() {
    Player *me = get_me();
    if (!(me->state.server_state.flags & PlayerServerFlagSpectating)) {
        TimesOfPlayer *top = get_times_of_player(me);
        if (top) {
            subsystem.set_color(0.75f, 0.75f, 1.0f, 0.75f);

            if (!game_over) {
                get_now(now_for_drawing);
            }
            ms_t d_ms = diff_ms(top->start_time, now_for_drawing);
            float diff = round(d_ms / 10.0f) / 100.0f;

            char *pb;
            int len;
            int x;
            int view_width = subsystem.get_view_width();
            Tileset *ts = resources.get_tileset("numbers");
            int tile_width = ts->get_tile(0)->get_tilegraphic()->get_width();
            int number_width = tile_width - 13;
            int i = static_cast<int>(floor(diff));
            int m = static_cast<int>((diff - i) * 100);

            /* first part */
            sprintf(buffer, "%d", i);
            len = strlen(buffer);
            x = view_width / 2 - (len * number_width) - number_width / 2;
            pb = buffer;
            while (*pb) {
                subsystem.draw_tile(ts->get_tile(*pb - '0'), x, 5);
                x += number_width;
                pb++;
            }

            /* draw dot */
            subsystem.draw_tile(ts->get_tile(11), x, 5);
            x += number_width;

            /* second part */
            sprintf(buffer, "%02d", m);
            pb = buffer;
            while (*pb) {
                subsystem.draw_tile(ts->get_tile(*pb - '0'), x, 5);
                x += number_width;
                pb++;
            }

            /* reset */
            subsystem.reset_color();

            /* draw best & last lap */
            size_t sz = top->times.size();
            Font *f = resources.get_font("big");

            const TimesOfPlayer& lead = times_of_players[0];
            if (lead.times.size()) {
                subsystem.set_color(0.25f, 1.0f, 0.25f, 1.0f);
                subsystem.draw_text(f, 5, 5, "lead:");
                sprintf(buffer, "%.2f (%s)", lead.best, lead.player->get_player_name().c_str());
                subsystem.draw_text(f, 45, 5, buffer);
                subsystem.reset_color();
            }

            subsystem.draw_text(f, 5, 20, "lap:");
            sprintf(buffer, "%d", static_cast<int>(sz + 1));
            subsystem.draw_text(f, 45, 20, buffer);

            if (sz) {
                subsystem.draw_text(f, 5, 35, "best:");
                sprintf(buffer, "%.2f", top->best);
                subsystem.draw_text(f, 45, 35, buffer);

                subsystem.draw_text(f, 5, 50, "last:");
                sprintf(buffer, "%.2f", top->last);
                subsystem.draw_text(f, 45, 50, buffer);
            }
        }
    }
}

void TournamentSR::reset_player(Player *p) {
    if (p == get_me()) {
        if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
            TimesOfPlayer *top = get_times_of_player(p);
            if (top) {
                top->running = true;
                get_now(top->start_time);
            }
        }
    }
}

GenericData *TournamentSR::create_generic_data() {
    GenericData *gd = 0;
    GenericData *lgd = 0;

    for (TimesOfPlayers::iterator it = times_of_players.begin(); it != times_of_players.end(); it++) {
        const TimesOfPlayer& top = *it;
        size_t sz = top.times.size();
        for (size_t i = 0; i < sz; i++) {
            GTransportTime *tm =  new GTransportTime;
            tm->id = top.player->state.id;
            tm->ms = static_cast<sr_milliseconds_t>(top.times[i] * 1000.0f);
            tm->to_net();
            GenericData *ngd = new GenericData(tm, GTransportTimeLen);
            if (lgd) {
                lgd->next = ngd;
                ngd = lgd;
            }
            if (!gd) {
                gd = ngd;
            }
        }
    }

    return gd;
}

void TournamentSR::destroy_generic_data(void *data) {
    GTransportTime *tm = reinterpret_cast<GTransportTime *>(data);
    delete tm;
}

void TournamentSR::generic_data_delivery(void *data) {
    score_transport_raw(data);
}

int TournamentSR::draw_stats(Font *f, int x, int y) {
    int rank = 0;
    float last_best_time = 0.0f;
    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = (*it);

        if (!(top.player->state.server_state.flags & PlayerServerFlagSpectating)) {
            bool has_entries = top.times.size() > 0;
            if (rank) {
                if (has_entries) {
                    if (top.best > last_best_time) {
                        rank++;
                    }
                } else {
                    rank++;
                }
            } else {
                rank++;
            }

            sprintf(buffer, "%d", rank);
            subsystem.draw_text(f, x, y, buffer);

            subsystem.draw_text(f, x + 20, y, top.player->get_player_name());

            sprintf(buffer, "%d", static_cast<int>(top.times.size()));
            subsystem.draw_text(f, x + 140, y, buffer);

            if (has_entries) {
                sprintf(buffer, "%.2f", top.best);
                subsystem.draw_text(f, x + 180, y, buffer);

                sprintf(buffer, "%.2f", top.last);
                subsystem.draw_text(f, x + 240, y, buffer);
                last_best_time = top.best;
            } else {
                subsystem.draw_text(f, x + 180, y, "N/A");
                subsystem.draw_text(f, x + 240, y, "N/A");
                last_best_time = 0.0f;
            }

            sprintf(buffer, "%d", top.player->state.server_state.ping_time);
            subsystem.draw_text(f, x + 300, y, buffer);

            y += f->get_font_height();
        }
    }

    return y;
}

TournamentSR::TimesOfPlayer *TournamentSR::get_times_of_player(Player *p) {
    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = *it;
        if (top.player == p) {
            return &top;
        }
    }

    return 0;
}

TournamentSR::TimesOfPlayer *TournamentSR::get_times_of_player(player_id_t id) {
    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = *it;
        if (top.player->state.id == id) {
            return &top;
        }
    }

    return 0;
}

void TournamentSR::update_stats(TimesOfPlayer *top, float t) {
    top->times.push_back(t);
    top->last = t;

    top->best = std::numeric_limits<float>::max();
    for (Times::iterator it = top->times.begin(); it != top->times.end(); it++) {
        float t = *it;
        if (t < top->best) {
            top->best = t;
        }
    }

    std::sort(times_of_players.begin(), times_of_players.end());
}