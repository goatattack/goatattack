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

#include "TournamentCTC.hpp"

#include <cstdlib>
#include <cmath>
#include <algorithm>

const int CoinDropInitialValue = 1000;
const int CoinInitialSendCounter = 15;

TournamentCTC::TournamentCTC(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : Tournament(resources, subsystem, gui, logger, game_file, server, map_name, players,
      duration, warmup), coin(0),
      enemy_indicator_coin(resources.get_icon("enemy_indicator_coin"))
{
    /* create spawn points */
    create_spawn_points();
    if (!spawn_points.size()) {
        throw TournamentException(i18n(I18N_MISSING_SPAWN_POINTS));
    }

    /* find coin in this map */
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        Object::ObjectType type = obj->object->get_type();
        if (type == Object::ObjectTypeCoin) {
            if (coin) {
                throw TournamentException(i18n(I18N_MULTIPLE_COINS));
            }
            coin = obj;
        }
    }

    if (!coin) {
        throw TournamentException(i18n(I18N_COIN_MISSING));
    }

    /* add all players into list */
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        times_of_players.push_back(TimesOfPlayer(p));
    }

    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_ctc");
}

TournamentCTC::~TournamentCTC() { }

const char *TournamentCTC::tournament_type() {
    return "CTC";
}

void TournamentCTC::write_stats_in_server_log() {
    int rank = 0;
    int total;
    int last_total = 0;
    int total_minutes;

    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = (*it);
        if (!(top.player->state.server_state.flags & PlayerServerFlagSpectating)) {
            total = static_cast<int>(top.total + top.current);
            if (!rank || last_total < total) {
                rank++;
            }
            last_total = total;
            total_minutes = total / 60;
            total -= total_minutes * 60;
            logger->log(ServerLogger::LogTypeStatsCTC, i18n(I18N_TNMT_STATS_PLAYER), top.player, 0,
                &rank, &total_minutes, &total);
        }
    }
}

void TournamentCTC::update_wearable_remaining(GTimeRemaining *remain) {
    coin->spawn_counter = remain->remaining;
}

void TournamentCTC::frag_point(Player *pfrag, Player *pkill) { }

void TournamentCTC::subintegrate(ns_t ns) {
    double period_f = ns / static_cast<double>(ns_fc);
    if (server) {
        check_coin_validity(period_f);

        if (!game_over) {
            for (Players::iterator it = players.begin(); it != players.end(); it++) {
                Player *p = *it;
                if (p->state.server_state.flags & PlayerServerFlagHasCoin) {
                    TimesOfPlayer *top = get_times_of_player(p);
                    if (top) {
                        gametime_t now;
                        get_now(now);
                        ms_t d_ms = diff_ms(top->start_time, now);
                        top->send_current = static_cast<sr_milliseconds_t>(d_ms);
                        top->current = round(d_ms / 10.0f) / 100.0f;
                        top->send_counter--;
                        if (!top->send_counter) {
                            top->send_counter = CoinInitialSendCounter;
                            send_coin_timer(top);
                        }
                    }
                }
            }
        }
    }

    /* now redirect to base */
    Tournament::subintegrate(ns);
}

void TournamentCTC::check_coin_validity(double period_f) {
    if (coin->state.y - 100 > map_height * tile_height) {
        /* check if coin is in valid map range */
        if (!coin->picked) {
            return_coin(PlaceObjectWithSpawnSound);
            add_i18n_response(I18N_TNMT_COIN_RETURNED);
        }
    } else if (coin_not_at_origin()) {
        /* check if coin is counting down (if dropped) */
        if (!coin->picked) {
            coin->spawn_counter -= period_f;
            if (!(static_cast<int>(coin->spawn_counter) % 25)) {
                send_coin_remaining();
            }
            if (coin->spawn_counter <= 0.0f) {
                return_coin(PlaceObjectWithSpawnSound);
                add_i18n_response(I18N_TNMT_COIN_RETURNED);
            }
        }
    }
}

void TournamentCTC::return_coin(int flags) {
    coin->picked = false;
    coin->state.accel_x = 0.0f;
    coin->state.accel_y = 0.0f;
    GPlaceObject *gpo = new GPlaceObject;
    memset(gpo, 0, sizeof(GPlaceObject));
    gpo->id = coin->state.id;
    gpo->flags = PlaceObjectWithAnimation | PlaceObjectResetVelocity | flags;
    gpo->x = coin->origin_x;
    gpo->y = coin->origin_y;
    add_place_object(gpo);
    gpo->to_net();
    add_state_response(GPCPlaceObject, sizeof(GPlaceObject), gpo);
}

bool TournamentCTC::coin_not_at_origin() {
    int dx = static_cast<int>(round(coin->state.x)) - coin->origin_x;
    int dy = static_cast<int>(round(coin->state.y)) - coin->origin_y;
    return dx || dy;
}

void TournamentCTC::send_coin_remaining() {
    GTimeRemaining *remain = new GTimeRemaining;
    remain->flags = 0;
    remain->remaining = static_cast<pos_t>(coin->spawn_counter);
    remain->to_net();
    add_state_response(GPCTimeRemaining, GTimeRemainingLen, remain);
}

void TournamentCTC::send_coin_timer(Player *p) {
    send_coin_timer(get_times_of_player(p));
}

void TournamentCTC::send_coin_timer(TimesOfPlayer *top) {
    if (top) {
        GTransportTotalAndTime *tm = new GTransportTotalAndTime;
        tm->id = top->player->state.id;
        tm->total = static_cast<sr_milliseconds_t>(round(top->total * 1000.0f));
        tm->current = top->send_current;
        tm->to_net();
        add_state_response(GPCScoreTransportRaw, GTransportTotalAndTimeLen, tm);
    }
}

void TournamentCTC::player_added(Player *p) {
    times_of_players.push_back(TimesOfPlayer(p));
}

void TournamentCTC::player_removed(Player *p) {
    test_and_drop_coin(p);

    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        if ((*it).player == p) {
            times_of_players.erase(it);
            break;
        }
    }

    Tournament::player_removed(p);
}

void TournamentCTC::player_died(Player *p) {
    if (test_and_drop_coin(p)) {
        TimesOfPlayer *top = get_times_of_player(p);
        if (top) {
            top->total += top->current;
            top->current = 0;
            top->send_current = 0;
            send_coin_timer(top);
        }
    }
}

bool TournamentCTC::pick_item(Player *p, GameObject *obj) {
    if (obj->object->get_type() == Object::ObjectTypeCoin) {
        p->state.server_state.flags |= PlayerServerFlagHasCoin;
        add_i18n_response(I18N_TNMT_COIN_CATCHED, p->get_player_name().c_str());
        if (logger) {
            logger->log(ServerLogger::LogTypeCoinPicked, i18n(I18N_TNMT_COIN_CATCHED, p->get_player_name().c_str()), p);
        }
        TimesOfPlayer *top = get_times_of_player(p);
        if (top) {
            get_now(top->start_time);
            top->send_counter = CoinInitialSendCounter;
        }
        return true;
    }

    return false;
}

void TournamentCTC::draw_object_addons() {
    /* draw coin return counter */
    if (coin_not_at_origin()) {
        if (!coin->picked) {
            int x = static_cast<int>(coin->state.x) + 2;
            int y = static_cast<int>(coin->state.y) - 10;
            int w = coin->object->get_tile()->get_tilegraphic()->get_width();
            subsystem.set_color(0.0f, 0.0f, 0.0f, 1.0f);
            subsystem.draw_box(x + left, y + top, w, 5);
            subsystem.set_color(1.0f, 1.0f, 1.0f, 1.0f);
            w = static_cast<int>(w * (coin->spawn_counter /
                static_cast<double>(CoinDropInitialValue)));
            subsystem.draw_box(x + left, y + top, w, 5);
            subsystem.reset_color();
        }
    }
}


void TournamentCTC::draw_player_addons() {
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        /* draw coin */
        Player *p = *it;
        if (p->is_alive_and_playing()) {
            if (p->state.server_state.flags & PlayerServerFlagHasCoin) {
                subsystem.draw_tile(coin->object->get_tile(),
                    static_cast<int>(p->state.client_server_state.x) + left + Characterset::CoinOffsetX,
                    static_cast<int>(p->state.client_server_state.y) + top + Characterset::CoinOffsetY);
            }
        }
    }
}

void TournamentCTC::draw_enemies_on_hud() {
    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    int ih = 8;
    int iw = 8;
    Player *me = get_me();

    subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p != me) {
            if (p->is_alive_and_playing()) {
                const CollisionBox& colbox = Characterset::DamageColbox;
                int cbw = colbox.width / 2;
                int cbh = colbox.height / 2;
                bool draw = false;
                int x = static_cast<int>(p->state.client_server_state.x) + left + colbox.x + cbw - iw / 2;
                int y = static_cast<int>(p->state.client_server_state.y) + top - colbox.y - cbh - ih / 2;
                if (x < 0) {
                    x = 0;
                    draw = true;
                }
                if (y < 0) {
                    draw = true;
                    y = 0;
                }
                if (x > view_width - ih) {
                    draw = true;
                    x = view_width - ih;
                }
                if (y > view_height - ih) {
                    draw = true;
                    y = view_height - ih;
                }

                if (draw) {
                    if (p->state.server_state.flags & PlayerServerFlagHasCoin) {
                        subsystem.draw_icon(enemy_indicator_coin, x, y);
                    } else {
                        subsystem.draw_icon(enemy_indicator, x, y);
                    }
                }
            }
        }
    }
    subsystem.reset_color();
}

void TournamentCTC::score_transport_raw(void *data) {
    GTransportTotalAndTime *tm = reinterpret_cast<GTransportTotalAndTime *>(data);
    tm->from_net();
    TimesOfPlayer *top = get_times_of_player(tm->id);
    if (top) {
        update_stats(top, tm->total, tm->current);
    }
}

void TournamentCTC::draw_statistics() {
    Font *font_normal = resources.get_font("normal");
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = ((vw - 20) / 2) - 40;
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
    std::string txt(i18n(I18N_TNMT_SB_CTC_TITLE));
    int tw = font_big->get_text_width(txt);
    subsystem.draw_text(font_big, vw / 2 - tw / 2, y + 18, txt);

    /* draw map name */
    std::string mapinfo(i18n(I18N_CLIENT_MAP_INFO, map.get_name().c_str()));
    int miw = font_normal->get_text_width(mapinfo);
    int mih = font_normal->get_font_height();
    subsystem.draw_text(font_normal, x + ww - miw - 15, y + wh - mih - 15, mapinfo);

    /* draw list */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    y = 55;
    x = wx + 15;
    subsystem.draw_text(font_normal, x, y, "#");
    subsystem.draw_text(font_normal, x + 20, y, i18n(I18N_TNMT_SB_PLAYER));
    subsystem.draw_text(font_normal, x + 140, y, i18n(I18N_TNMT_SB_TOTAL));
    subsystem.draw_text(font_normal, x + 220, y, i18n(I18N_TNMT_SB_PING));
    subsystem.reset_color();

    y = draw_stats(font_normal, wx + 15, y + 15);

    /* spectators */
    x = wx + 15;
    y += 50;

    /* done */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_text(font_normal, wx + 15, y, i18n(I18N_TNMT_SB_SPECTATORS));
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

void TournamentCTC::draw_score() {
    Player *me = get_me();
    if (!(me->state.server_state.flags & PlayerServerFlagSpectating)) {
        TimesOfPlayer *top = get_times_of_player(me);
        if (top) {
            int total_seconds = static_cast<int>(top->total + top->current);
            int total_minutes = total_seconds / 60;
            total_seconds -= (total_minutes * 60);

            subsystem.set_color(0.75f, 0.75f, 1.0f, 0.75f);

            char *pb;
            int len;
            int x;
            int view_width = subsystem.get_view_width();
            Tileset *ts = resources.get_tileset("numbers");
            int tile_width = ts->get_tile(0)->get_tilegraphic()->get_width();
            int number_width = tile_width - 13;

            /* first part */
            sprintf(buffer, "%d", total_minutes);
            len = strlen(buffer);
            x = view_width / 2 - (len * number_width) - number_width + 4;
            pb = buffer;
            while (*pb) {
                subsystem.draw_tile(ts->get_tile(*pb - '0'), x, 5);
                x += number_width;
                pb++;
            }

            /* draw colon */
            subsystem.draw_tile(ts->get_tile(10), x, 5);
            x += number_width;

            /* second part */
            sprintf(buffer, "%02d", total_seconds);
            pb = buffer;
            while (*pb) {
                subsystem.draw_tile(ts->get_tile(*pb - '0'), x, 5);
                x += number_width;
                pb++;
            }

            /* reset */
            subsystem.reset_color();

            /* draw best & last lap */
            Font *f = resources.get_font("big");

            const TimesOfPlayer& lead = times_of_players[0];
            float diff = lead.total + lead.current;
            total_seconds = static_cast<int>(diff);
            total_minutes = total_seconds / 60;
            total_seconds -= (total_minutes * 60);
            if (round(diff * 100.0f) > 0.0f) {
                subsystem.set_color(0.25f, 1.0f, 0.25f, 1.0f);
                sprintf(buffer, "%s: %d:%02d", lead.player->get_player_name().c_str(), total_minutes, total_seconds);
                subsystem.draw_text(f, 5, 5, buffer);
                subsystem.reset_color();
            }
        }
    }
}

GenericData *TournamentCTC::create_generic_data() {
    GenericData *gd = 0;
    GenericData *lgd = 0;

    for (TimesOfPlayers::iterator it = times_of_players.begin(); it != times_of_players.end(); it++) {
        const TimesOfPlayer& top = *it;
        GTransportTotalAndTime *tm = new GTransportTotalAndTime;
        tm->id = top.player->state.id;
        tm->total = static_cast<sr_milliseconds_t>(round(top.total * 1000.0f));
        tm->current = top.send_current;
        tm->to_net();
        GenericData *ngd = new GenericData(tm, GTransportTotalAndTimeLen);
        if (lgd) {
            lgd->next = ngd;
            ngd = lgd;
        }
        if (!gd) {
            gd = ngd;
        }
    }

    return gd;
}

void TournamentCTC::destroy_generic_data(void *data) {
    GTransportTotalAndTime *tm = reinterpret_cast<GTransportTotalAndTime *>(data);
    delete tm;
}

void TournamentCTC::generic_data_delivery(void *data) {
    score_transport_raw(data);
}

bool TournamentCTC::test_and_drop_coin(Player *p) {
    /* test if player has coin -> drop it */
    bool dropped = false;
    if (server) {
        if (p->state.server_state.flags & PlayerServerFlagHasCoin) {
            /* drop object */
            p->state.server_state.flags &= ~PlayerServerFlagHasCoin;
            coin->picked = false;
            coin->spawn_counter = static_cast<double>(CoinDropInitialValue);
            send_coin_remaining();

            coin->state.accel_x = p->state.client_server_state.accel_x;
            coin->state.accel_y = p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y;

            GPlaceObject *gpo = new GPlaceObject;
            memset(gpo, 0, sizeof(GPlaceObject));
            gpo->id = coin->state.id;
            gpo->flags = PlaceObjectWithDropSound;
            gpo->x = static_cast<pos_t>(p->state.client_server_state.x) + Characterset::CoinDropOffsetX;
            gpo->y = static_cast<pos_t>(p->state.client_server_state.y) + Characterset::CoinDropOffsetY;
            add_place_object(gpo);
            gpo->to_net();
            add_state_response(GPCPlaceObject, sizeof(GPlaceObject), gpo);

            /* add message */
            add_i18n_response(I18N_TNMT_COIN_DROPPED);
            dropped = true;

            /* log */
            if (logger) {
                logger->log(ServerLogger::LogTypeCoinDropped, i18n(I18N_TNMT_COIN_DROPPED), p);
            }
        }
    }

    return dropped;
}

TournamentCTC::TimesOfPlayer *TournamentCTC::get_times_of_player(Player *p) {
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

TournamentCTC::TimesOfPlayer *TournamentCTC::get_times_of_player(player_id_t id) {
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

void TournamentCTC::update_stats(TimesOfPlayer *top, sr_milliseconds_t total, sr_milliseconds_t current) {
    if (top) {
        top->total = static_cast<float>(total) / 1000.0f;
        top->send_current = current;
        top->current = static_cast<float>(current) / 1000.0f;

        std::sort(times_of_players.begin(), times_of_players.end());
    }
}

int TournamentCTC::draw_stats(Font *f, int x, int y) {
    int rank = 0;
    int total;
    int last_total = 0;
    int total_minutes;

    for (TimesOfPlayers::iterator it = times_of_players.begin();
        it != times_of_players.end(); it++)
    {
        TimesOfPlayer& top = (*it);

        if (!(top.player->state.server_state.flags & PlayerServerFlagSpectating)) {
            total = static_cast<int>(top.total + top.current);
            if (!rank || last_total < total) {
                rank++;
            }
            last_total = total;
            total_minutes = total / 60;
            total -= total_minutes * 60;

            sprintf(buffer, "%d", rank);
            subsystem.draw_text(f, x, y, buffer);

            subsystem.draw_text(f, x + 20, y, top.player->get_player_name());

            sprintf(buffer, "%d:%02d", total_minutes, total);
            subsystem.draw_text(f, x + 140, y, buffer);

            sprintf(buffer, "%d", top.player->state.server_state.ping_time);
            subsystem.draw_text(f, x + 220, y, buffer);

            y += f->get_font_height();
        }
    }

    return y;
}
