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

#include "TournamentCTF.hpp"

#include <cstdlib>
#include <cmath>

namespace {

    int FlagDropInitialValue = 1000;
    int FlagPickRefusal = 100;

}

TournamentCTF::TournamentCTF(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    : TournamentTeam(resources, subsystem, gui, logger, game_file, server, map_name, players,
      duration, warmup), red_flag(0), blue_flag(0)
{
    /* find red and blue flags in this map */
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        Object::ObjectType type = obj->object->get_type();
        if (type == Object::ObjectTypeRedFlag) {
            if (red_flag) {
                throw TournamentException(i18n(I18N_MULTIPLE_RED_FLAGS));
            }
            red_flag = obj;
        } else if (type == Object::ObjectTypeBlueFlag) {
            if (blue_flag) {
                throw TournamentException(i18n(I18N_MULTIPLE_BLUE_FLAGS));
            }
            blue_flag = obj;
        }
    }

    if (!red_flag) {
        throw TournamentException(i18n(I18N_RED_FLAG_MISSING));
    }

    if (!blue_flag) {
        throw TournamentException(i18n(I18N_BLUE_FLAG_MISSING));
    }

    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_ctf");
}

TournamentCTF::~TournamentCTF() { }

const char *TournamentCTF::tournament_type() {
    return "CTF";
}

std::string TournamentCTF::get_game_type_name() const {
    return i18n(I18N_MAINMENU_GM_CTF);
}

void TournamentCTF::subintegrate(ns_t ns) {
    double period_f = ns / static_cast<double>(ns_fc);
    if (server) {
        check_flag_validity(period_f, I18N_TNMT_RED_FLAG_RETURNED, red_flag);
        check_flag_validity(period_f, I18N_TNMT_BLUE_FLAG_RETURNED, blue_flag);

        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;

            /* check if player wants to drop the flag */
            bool move_drop = ((p->state.client_server_state.key_states & PlayerKeyStateFire) != 0);
            if (move_drop) {
                if (test_and_drop_flag(p)) {
                    p->flag_pick_refused_counter = FlagPickRefusal;
                    p->flag_pick_refused = true;
                }
            }

            /* decrement flag pick refusal counter */
            if (p->flag_pick_refused) {
                p->flag_pick_refused_counter -= period_f;
                if (p->flag_pick_refused_counter <= 0.0) {
                    p->flag_pick_refused = false;
                }
            }
        }
    }

    /* now redirect to base */
    TournamentTeam::subintegrate(ns);
}

void TournamentCTF::check_flag_validity(double period_f, I18NText id, GameObject *flag) {
    if (flag->state.y - 100 > map_height * tile_height) {
        /* check if flags are in valid map range */
        if (!flag->picked) {
            return_flag(flag, PlaceObjectWithSpawnSound);
            add_i18n_response(id);
            if (logger) {
                logger->log(flag->object->get_type() == Object::ObjectTypeRedFlag
                    ? ServerLogger::LogTypeRedFlagReturned
                    : ServerLogger::LogTypeBlueFlagReturned, i18n(id));
            }
        }
    } else if (flag_not_at_origin(flag)) {
        /* check if flag is counting down (if dropped) */
        if (!flag->picked) {
            flag->spawn_counter -= period_f;
            if (!(static_cast<int>(flag->spawn_counter) % 25)) {
                send_flag_remaining(flag);
            }
            if (flag->spawn_counter <= 0.0f) {
                return_flag(flag, PlaceObjectWithSpawnSound);
                add_i18n_response(id);
                if (logger) {
                    logger->log(flag->object->get_type() == Object::ObjectTypeRedFlag
                        ? ServerLogger::LogTypeRedFlagReturned
                        : ServerLogger::LogTypeBlueFlagReturned, i18n(id));
                }
            }
        }
    }
}

void TournamentCTF::draw_object_addons() {
    /* draw flag return counter */
    GameObject *flag;
    for (int i = 0; i < 2; i++) {
        flag = (!i ? red_flag : blue_flag);
        if (flag_not_at_origin(flag)) {
            if (!flag->picked) {
                int x = static_cast<int>(flag->state.x);
                int y = static_cast<int>(flag->state.y) - 10;
                int w = flag->object->get_tile()->get_tilegraphic()->get_width();
                subsystem.set_color(0.0f, 0.0f, 0.0f, 1.0f);
                subsystem.draw_box(x + left, y + top, w, 5);
                subsystem.set_color(1.0f, 1.0f, 1.0f, 1.0f);
                w = static_cast<int>(w * (flag->spawn_counter /
                    static_cast<double>(FlagDropInitialValue)));
                subsystem.draw_box(x + left, y + top, w, 5);
                subsystem.reset_color();
            }
        }
    }
}

void TournamentCTF::draw_player_addons() {
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        /* draw flag */
        Player *p = *it;
        if (p->is_alive_and_playing()) {
            if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag) {
                GameObject *flag = 0;
                if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                    flag = blue_flag;
                } else {
                    flag = red_flag;
                }
                subsystem.draw_tile(flag->object->get_tile(),
                    static_cast<int>(p->state.client_server_state.x) + left + Characterset::FlagOffsetX,
                    static_cast<int>(p->state.client_server_state.y) + top + Characterset::FlagOffsetY);
            }
        }
    }
}

void TournamentCTF::player_removed(Player *p) {
    test_and_drop_flag(p);
    Tournament::player_removed(p);
}

void TournamentCTF::player_died(Player *p) {
    test_and_drop_flag(p);
}

bool TournamentCTF::play_gun_error(Player *p) {
    if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag) {
        return false;
    }
    return true;
}

bool TournamentCTF::pick_item(Player *p, GameObject *obj) {
    switch (obj->object->get_type()) {
        case Object::ObjectTypeRedFlag:
            if (!p->flag_pick_refused) {
                if (!(p->state.server_state.flags & PlayerServerFlagTeamRed)) {
                    /* team blue catches red flag */
                    p->state.server_state.flags |= PlayerServerFlagHasOppositeFlag;
                    add_i18n_response(I18N_TNMT_RED_FLAG_CATCHED, p->get_player_name().c_str());
                    if (logger) {
                        logger->log(ServerLogger::LogTypeRedFlagPicked, i18n(I18N_TNMT_RED_FLAG_CATCHED, p->get_player_name()), p);
                    }
                    return true;
                } else {
                    /* team red saves red flag */
                    if (obj->state.x != obj->origin_x || obj->state.y != obj->origin_y) {
                        return_flag(obj, PlaceObjectWithSpawnSound);
                        p->state.server_state.score += 2;
                        add_i18n_response(I18N_TNMT_RED_FLAG_SAVED, p->get_player_name().c_str());
                        if (logger) {
                            logger->log(ServerLogger::LogTypeRedFlagSaved, i18n(I18N_TNMT_RED_FLAG_SAVED, p->get_player_name()), p);
                        }
                    }
                }
            }
            break;

        case Object::ObjectTypeBlueFlag:
            if (!p->flag_pick_refused) {
                if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                    /* team blue catches blue flag */
                    p->state.server_state.flags |= PlayerServerFlagHasOppositeFlag;
                    add_i18n_response(I18N_TNMT_BLUE_FLAG_CATCHED, p->get_player_name().c_str());
                    if (logger) {
                        logger->log(ServerLogger::LogTypeBlueFlagPicked, i18n(I18N_TNMT_BLUE_FLAG_CATCHED, p->get_player_name().c_str()), p);
                    }
                    return true;
                } else {
                    /* team blue saves blue flag */
                    if (obj->state.x != obj->origin_x || obj->state.y != obj->origin_y) {
                        return_flag(obj, PlaceObjectWithSpawnSound);
                        p->state.server_state.score += 2;
                        add_i18n_response(I18N_TNMT_BLUE_FLAG_RETURNED, p->get_player_name().c_str());
                        if (logger) {
                            logger->log(ServerLogger::LogTypeBlueFlagSaved, i18n(I18N_TNMT_BLUE_FLAG_RETURNED, p->get_player_name().c_str()), p);
                        }
                    }
                }
            }
            break;

        default:
            break;
    }

    return false;
}

bool TournamentCTF::tile_collision(TestType type, Player *p, int last_falling_y_pos,
    Tile *t, bool *killing)
{
    if (killing) *killing = false;

    if (p) {
        switch (t->get_tile_type()) {
            case Tile::TileTypeBaseRed:
                if (server && p) {
                    if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag &&
                        p->state.server_state.flags & PlayerServerFlagTeamRed)
                    {
                        /* red team scores */
                        p->state.server_state.flags &= ~PlayerServerFlagHasOppositeFlag;
                        p->state.server_state.score += 3;
                        return_flag(blue_flag, PlaceObjectWithScoredSound);
                        add_i18n_response(I18N_TNMT_TEAM_RED_SCORED1);
                        if (logger) {
                            logger->log(ServerLogger::LogTypeTeamRedScored, i18n(I18N_TNMT_TEAM_RED_SCORED1), p);
                        }
                        add_team_score_animation(p, I18N_TNMT_TEAM_RED_SCORED2);
                        score.score_red++;
                        send_team_score();
                    }
                }
                return true;
                break;

            case Tile::TileTypeBaseBlue:
                if (server && p) {
                    if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag &&
                        !(p->state.server_state.flags & PlayerServerFlagTeamRed))
                    {
                        /* blue team scores */
                        p->state.server_state.flags &= ~PlayerServerFlagHasOppositeFlag;
                        p->state.server_state.score += 3;
                        return_flag(red_flag, PlaceObjectWithScoredSound);
                        add_i18n_response(I18N_TNMT_TEAM_BLUE_SCORED1);
                        if (logger) {
                            logger->log(ServerLogger::LogTypeTeamBlueScored, i18n(I18N_TNMT_TEAM_BLUE_SCORED1), p);
                        }
                        add_team_score_animation(p, I18N_TNMT_TEAM_BLUE_SCORED2);
                        score.score_blue++;
                        send_team_score();
                    }
                }
                return true;
                break;

            default:
                break;
        }
    }

    return Tournament::tile_collision(type, p, last_falling_y_pos, t, killing);
}

bool TournamentCTF::test_and_drop_flag(Player *p) {
    /* test if player has flag -> drop it */
    bool dropped = false;
    if (server) {
        if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag) {
            GameObject *flag = 0;
            if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                flag = blue_flag;
            } else {
                flag = red_flag;
            }

            /* drop object */
            p->state.server_state.flags &= ~PlayerServerFlagHasOppositeFlag;
            flag->picked = false;
            flag->spawn_counter = static_cast<double>(FlagDropInitialValue);
            send_flag_remaining(flag);

            flag->state.accel_x = p->state.client_server_state.accel_x;
            flag->state.accel_y = p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y;

            GPlaceObject *gpo = new GPlaceObject;
            memset(gpo, 0, sizeof(GPlaceObject));
            gpo->id = flag->state.id;
            gpo->flags = PlaceObjectWithDropSound;
            gpo->x = static_cast<pos_t>(p->state.client_server_state.x) + Characterset::FlagDropOffsetX;
            gpo->y = static_cast<pos_t>(p->state.client_server_state.y) + Characterset::FlagDropOffsetY;
            add_place_object(gpo);
            gpo->to_net();
            add_state_response(GPCPlaceObject, sizeof(GPlaceObject), gpo);

            /* add message */
            if (flag == red_flag) {
                add_i18n_response(I18N_TNMT_RED_FLAG_DROPPED);
                if (logger) {
                    logger->log(ServerLogger::LogTypeRedFlagDropped, i18n(I18N_TNMT_RED_FLAG_DROPPED), p);
                }
            } else {
                add_i18n_response(I18N_TNMT_BLUE_FLAG_DROPPED);
                if (logger) {
                    logger->log(ServerLogger::LogTypeBlueFlagDropped, i18n(I18N_TNMT_BLUE_FLAG_DROPPED), p);
                }
            }
            dropped = true;
        }
    }

    return dropped;
}

void TournamentCTF::return_flag(GameObject *flag, int flags) {
    flag->picked = false;
    flag->state.accel_x = 0.0f;
    flag->state.accel_y = 0.0f;
    GPlaceObject *gpo = new GPlaceObject;
    memset(gpo, 0, sizeof(GPlaceObject));
    gpo->id = flag->state.id;
    gpo->flags = PlaceObjectWithAnimation | PlaceObjectResetVelocity | flags;
    gpo->x = flag->origin_x;
    gpo->y = flag->origin_y;
    add_place_object(gpo);
    gpo->to_net();
    add_state_response(GPCPlaceObject, sizeof(GPlaceObject), gpo);
}

bool TournamentCTF::fire_enabled(Player *p) {
    return !(p->state.server_state.flags & PlayerServerFlagHasOppositeFlag);
}

bool TournamentCTF::flag_not_at_origin(GameObject *flag) {
    int dx = static_cast<int>(round(flag->state.x)) - flag->origin_x;
    int dy = static_cast<int>(round(flag->state.y)) - flag->origin_y;
    return dx || dy;
}

void TournamentCTF::send_flag_remaining(GameObject *flag) {
    GTimeRemaining *remain = new GTimeRemaining;
    remain->flags = (flag->object->get_type() == Object::ObjectTypeRedFlag ? 1 : 0);
    remain->remaining = static_cast<pos_t>(flag->spawn_counter);
    remain->to_net();
    add_state_response(GPCTimeRemaining, GTimeRemainingLen, remain);
}

void TournamentCTF::update_wearable_remaining(GTimeRemaining *remain) {
    GameObject *flag = (remain->flags ? red_flag : blue_flag);
    flag->spawn_counter = remain->remaining;
}

void TournamentCTF::team_fire_point(Player *penem, Team team, int points) { }
