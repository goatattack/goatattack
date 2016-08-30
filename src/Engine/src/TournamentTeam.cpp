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

#include "TournamentTeam.hpp"

#include <algorithm>
#include <cstdlib>

const int CheckUnbalancedInitialValue = 10000;

TournamentTeam::TournamentTeam(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : Tournament(resources, subsystem, gui, logger, game_file, server, map_name,
      players, duration, warmup),
      team_badge_red(resources.get_icon("team_badge_red")),
      team_badge_blue(resources.get_icon("team_badge_blue")),
      enemy_indicator_red(resources.get_icon("enemy_indicator_red")),
      enemy_indicator_blue(resources.get_icon("enemy_indicator_blue")),
      enemy_indicator_red_flag(resources.get_icon("enemy_indicator_red_flag")),
      enemy_indicator_blue_flag(resources.get_icon("enemy_indicator_blue_flag")),
      check_unbalancing(CheckUnbalancedInitialValue), choose_team_open(false),
      nav(0)
{
    /* create spawn points */
    create_spawn_points();
}

TournamentTeam::~TournamentTeam() {
    if (choose_team_open) {
        if (!gui_is_destroyed) {
            gui->pop_window();
        }
    }
    if (nav) {
        delete nav;
    }
}

void TournamentTeam::create_spawn_points() throw (TournamentException) {
    for (GameObjects::iterator it = game_objects.begin(); it != game_objects.end(); it++) {
        GameObject *obj = *it;
        Object::ObjectType type = obj->object->get_type();
        if (type == Object::ObjectTypeSpawnPointRed) {
            spawn_points_red.push_back(obj);
        } else  if (type == Object::ObjectTypeSpawnPointBlue) {
            spawn_points_blue.push_back(obj);
        }
    }

    if (!spawn_points_red.size()) {
        throw TournamentException("Missing spawn points for " + team_red_name + " in this map.");
    }

    if (!spawn_points_blue.size()) {
        throw TournamentException("Missing spawn points for " + team_blue_name + " in this map.");
    }
}

void TournamentTeam::spawn_player(Player *p) {
    if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
        spawn_player_base(p, spawn_points_red);
    } else {
        spawn_player_base(p, spawn_points_blue);
    }
}

void TournamentTeam::draw_team_colours() {
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->is_alive_and_playing()) {
            /* draw team colours */
            Tile *t = p->get_characterset()->get_tile(
                static_cast<Direction>(p->state.client_server_state.direction),
                static_cast<CharacterAnimation>(p->state.client_state.icon));
            TileGraphic *tg = t->get_tilegraphic();
            const int bar_width = 32;
            const int bar_height = 4;
            int x = static_cast<int>(p->state.client_server_state.x) + (tg->get_width() / 2) - (bar_width / 2);
            int y = static_cast<int>(p->state.client_server_state.y) - (tg->get_height()) - bar_height + 2;
            if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                subsystem.set_color(1.0f, 0.0f, 0.0f, 1.0f);
            } else {
                subsystem.set_color(0.0f, 0.0f, 1.0f, 1.0f);
            }
            subsystem.draw_box(x + left, y + top, bar_width, bar_height);
            subsystem.reset_color();
        }
    }
}

void TournamentTeam::draw_statistics() {
    Font *font_normal = resources.get_font("normal");
    Font *font_big = resources.get_font("big");
    int x = 10;
    int y = 10;
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = vw - 20;
    int wh = vh - 20;

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
    subsystem.draw_box(20, 25, ww / 2 - 15, 42);
    subsystem.draw_box(ww / 2 + 15, 25, ww / 2 - 15, 42);

    /* reset */
    subsystem.reset_color();

    /* draw screws */
    subsystem.draw_icon(screw1, 15, 15);
    subsystem.draw_icon(screw2, vw - 15 - 8, 15);
    subsystem.draw_icon(screw3, 15, vh - 15 - 8);
    subsystem.draw_icon(screw4, vw - 15 - 8, vh - 15 - 8);

    /* draw team headers */
    std::string uc_team_red = uppercase(team_red_name);
    std::string uc_team_blue = uppercase(team_blue_name);
    subsystem.draw_text(font_big, 30, 38, uc_team_red.c_str());
    int rh_width = font_big->get_text_width(uc_team_blue);
    subsystem.draw_text(font_big, vw - 35 - rh_width, 38, uc_team_blue.c_str());

    /* draw team badges and score */
    subsystem.draw_tilegraphic(team_badge_red->get_tile()->get_tilegraphic(), vw / 2 - 34 - 10, 30);
    subsystem.draw_tilegraphic(team_badge_blue->get_tile()->get_tilegraphic(), vw / 2 + 2 + 10, 30);
    draw_team_score(vw / 2 - 66 - 10, 30, true, score.score_red);
    draw_team_score(vw / 2 + 34 + 10, 30, false, score.score_blue);

    /* draw list */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    y = 75;
    for (int i = 0; i < 2; i++) {
        x = (i ? 25 : vw / 2 + 10);
        subsystem.draw_text(font_normal, x, y, "#");
        subsystem.draw_text(font_normal, x + 20, y, "PLAYER");
        subsystem.draw_text(font_normal, x + 140, y, "SCORE");
        subsystem.draw_text(font_normal, x + 180, y, "FRAGS");
        subsystem.draw_text(font_normal, x + 220, y, "KILLS");
        subsystem.draw_text(font_normal, x + 260, y, "PING");
    }
    subsystem.reset_color();

    int y1 = draw_team_stats(font_normal, 25, 90, PlayerServerFlagTeamRed);
    int y2 = draw_team_stats(font_normal, vw / 2 + 10, 90, 0);

    /* spectators */
    y = (y1 > y2 ? y1 : y2) + 50;
    x = 25;

    /* done */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_text(font_normal, 25, y, "SPECTACTORS:");
    subsystem.reset_color();
    y += font_normal->get_font_height();
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->state.server_state.flags & PlayerServerFlagSpectating) {
            int w = font_normal->get_text_width(p->get_player_name());
            if (x + w >= vw - 25) {
                x = 25;
                y += font_normal->get_font_height();
            }
            subsystem.draw_text(font_normal, x, y, p->get_player_name());
            x += w + 10;
        }
    }
}

int TournamentTeam::draw_team_stats(Font *f, int x, int y, playerflags_t flags) {
    char buffer[16];
    std::vector<Player *> ranking;

    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if ((p->state.server_state.flags & PlayerServerFlagTeamRed) == flags) {
            if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
                ranking.push_back(p);
            }
        }
    }

    std::sort(ranking.begin(), ranking.end(), ComparePlayerScore);

    int rank = 0;
    sscore_t last_score;
    sscore_t current_score;
    for (std::vector<Player *>::iterator it = ranking.begin(); it != ranking.end(); it++) {
        Player *p = *it;
        current_score = p->state.server_state.score;
        if (rank) {
            if (current_score != last_score) {
                rank++;
            }
        } else {
            rank++;
        }

        last_score = current_score;

        sprintf(buffer, "%d", rank);
        subsystem.draw_text(f, x, y, buffer);

        subsystem.draw_text(f, x + 20, y, p->get_player_name());

        sprintf(buffer, "%d", current_score);
        subsystem.draw_text(f, x + 140, y, buffer);

        sprintf(buffer, "%d", p->state.server_state.frags);
        subsystem.draw_text(f, x + 180, y, buffer);

        sprintf(buffer, "%d", p->state.server_state.kills);
        subsystem.draw_text(f, x + 220, y, buffer);

        sprintf(buffer, "%d", p->state.server_state.ping_time);
        subsystem.draw_text(f, x + 260, y, buffer);

        y += f->get_font_height();
    }

    return y;
}

void TournamentTeam::draw_team_score(int x, int y, bool rightalign, score_t score) {
    /* draw team score */
    char buffer[16];
    char *pb;
    Tileset *ts = resources.get_tileset("numbers");
    int tile_width = ts->get_tile(0)->get_tilegraphic()->get_width();
    int number_width = tile_width - 13;

    /* team red */
    sprintf(buffer, "%d", score);
    if (rightalign) {
        x -= (strlen(buffer) - 1) * number_width;
    }
    pb = buffer;
    while (*pb) {
        subsystem.draw_tile(ts->get_tile(*pb - '0'), x, y);
        x += number_width;
        pb++;
    }
}

void TournamentTeam::retrieve_states() {
    send_team_score();
}

void TournamentTeam::add_team_score(GTeamScore *ts) {
    score = *ts;
}

void TournamentTeam::send_team_score() {
    GTeamScore *ts = new GTeamScore;
    *ts = score;
    ts->to_net();
    add_state_response(GPCTeamScore, sizeof(GTeamScore), ts);
}

void TournamentTeam::frag_point(Player *pfrag, Player *pkill) {
    Team team = (pfrag->state.server_state.flags & PlayerServerFlagTeamRed ? TeamRed : TeamBlue);
    if ((pfrag->state.server_state.flags & PlayerServerFlagTeamRed) ==
        (pkill->state.server_state.flags & PlayerServerFlagTeamRed))
    {
        /* friendly fire */
        pfrag->state.server_state.score -= 2;
    } else {
        /* regular frag */
        pfrag->state.server_state.score += 1;
        team_fire_point(pkill, team, 1);
    }
}

void TournamentTeam::player_join_request(Player *p) {
    if (!p->joining) {
        p->joining = true;
        std::string info("Choose with up and down, select with fire.");
        TileGraphic *tgr = team_badge_red->get_tile()->get_tilegraphic();
        TileGraphic *tgb = team_badge_blue->get_tile()->get_tilegraphic();
        int vw = subsystem.get_view_width();
        int vh = subsystem.get_view_height();
        Font *f = resources.get_font("normal");
        int ww = f->get_text_width(info) + 2 * Gui::Spc;
        int bh = tgr->get_height() + 5;
        int fh = f->get_font_height();
        int wh = 3 * Gui::Spc + 10 + 2 * bh + 2 * fh + 10;
        GuiButton *btn;

        if (!nav) {
            nav = new ButtonNavigator(*gui, *player_configuration);
        }

        nav->clear();
        GuiWindow *window = gui->push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, "Select Team");
        gui->create_label(window, Gui::Spc, Gui::Spc, info);

        btn = gui->create_button(window, Gui::Spc, Gui::Spc + 15 + Gui::Spc, ww - (2 * Gui::Spc), bh, uppercase(team_red_name), static_red_team_click, this);
        btn->show_bolts(false);
        gui->create_picture(btn, 2, 2, tgr);
        nav->add_button(btn);

        btn = gui->create_button(window, Gui::Spc, Gui::Spc + 15 + Gui::Spc + 10 + bh, ww - (2 * Gui::Spc), bh, uppercase(team_blue_name), static_blue_team_click, this);
        btn->show_bolts(false);
        gui->create_picture(btn, 1, 1, tgb);
        nav->add_button(btn);

        choose_team_open = true;
        nav->install_handlers(window, 0, 0);
        nav->set_button_focus();
    }
}

void TournamentTeam::static_red_team_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<TournamentTeam *>(data))->team_click(PlayerServerFlagTeamRed);
}

void TournamentTeam::static_blue_team_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<TournamentTeam *>(data))->team_click(0);
}

void TournamentTeam::team_click(playerflags_t flags) {
    close_select_team_window();
    Player *me = get_me();
    if (me) {
        me->state.server_state.flags |= flags;
        this->spawn_player(me);
        playerflags_t *pflags = new playerflags_t;
        *pflags = flags;
        add_state_response(GPSJoinRequest, sizeof(playerflags_t), pflags);
    }
}

void TournamentTeam::close_select_team_window() {
    gui->pop_window();
    choose_team_open = false;
    if (nav) {
        delete nav;
        nav = 0;
    }
}

bool TournamentTeam::player_joins(Player *p, playerflags_t flags) {
    std::string msg(p->get_player_name() + " joins the ");
    if (flags & PlayerServerFlagTeamRed) {
        msg += team_red_name;
        p->state.server_state.flags |= PlayerServerFlagTeamRed;
        if (logger) {
            logger->log(ServerLogger::LogTypeRedTeamJoin, msg, p);
        }
    } else {
        msg += team_blue_name;
        if (logger) {
            logger->log(ServerLogger::LogTypeBlueTeamJoin, msg, p);
        }
    }
    add_msg_response(msg.c_str());

    return true;
}

void TournamentTeam::check_friendly_fire(identifier_t owner, Player *p) {
    if (do_friendly_fire_alarm && owner) {
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *fp = *it;
            if (fp->state.id == owner) {
                if (p != fp) {
                    if ((fp->state.server_state.flags & PlayerServerFlagTeamRed) ==
                        (p->state.server_state.flags & PlayerServerFlagTeamRed))
                    {
                        GFriendyFireAlarm *alarm = new GFriendyFireAlarm;
                        alarm->owner = owner;
                        alarm->to_net();
                        add_state_response(GPCFriendlyFire, GFriendyFireAlarmLen, alarm);
                    }
                }
                break;
            }
        }
    }
}

bool TournamentTeam::friendly_fire_alarm(GFriendyFireAlarm *alarm) {
    Player *me = get_me();
    if (me && me->state.id == alarm->owner) {
        return true;
    }

    return false;
}

void TournamentTeam::draw_enemies_on_hud() {
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
                const CollisionBox& colbox = p->get_characterset()->get_damage_colbox();
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
                    if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                        if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag) {
                            subsystem.draw_icon(enemy_indicator_red_flag, x, y);
                        } else {
                            subsystem.draw_icon(enemy_indicator_red, x, y);
                        }
                    } else {
                        if (p->state.server_state.flags & PlayerServerFlagHasOppositeFlag) {
                            subsystem.draw_icon(enemy_indicator_blue_flag, x, y);
                        } else {
                            subsystem.draw_icon(enemy_indicator_blue, x, y);
                        }
                    }
                }
            }
        }
    }
    subsystem.reset_color();
}

void TournamentTeam::subintegrate(ns_t ns) {
    double period_f = ns / static_cast<double>(ns_fc);

    /* check if gameplay is unbalanced */
    if (server) {
        check_unbalancing -= period_f;
        if (check_unbalancing <= 0.0f) {
            check_unbalancing = CheckUnbalancedInitialValue;
            int cnt_team_red = 0;
            int cnt_team_blue = 0;
            for (Players::iterator it = players.begin(); it != players.end(); it++) {
                Player *p = *it;
                if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
                    if (p->state.server_state.flags & PlayerServerFlagTeamRed) {
                        cnt_team_red++;
                    } else {
                        cnt_team_blue++;
                    }
                }
            }
            if (cnt_team_red + cnt_team_blue > 1) {
                if (cnt_team_red - cnt_team_blue) {
                    add_state_response(GPCGamePlayUnbalanced, 0, 0);
                }
            }
        }
    }
}

void TournamentTeam::add_team_score_animation(Player *p, const std::string& text) {
    Font *font = resources.get_font("big");
    int tw = font->get_text_width(text);

    GTextAnimation *tani = new GTextAnimation;
    memset(tani, 0, sizeof(GTextAnimation));
    strncpy(tani->font_name, font->get_name().c_str(), NameLength - 1);
    strncpy(tani->display_text, text.c_str(), TextLength - 1);
    tani->max_counter = 65;
    tani->x = p->state.client_server_state.x + p->get_characterset()->get_width() / 2 - tw / 2;
    tani->y = p->state.client_server_state.y;
    tani->to_net();
    add_state_response(GPCAddTextAnimation, sizeof(GTextAnimation), tani);
}

void TournamentTeam::reopen_join_window(Player *p) {
    p->joining = false;
    player_join_request(p);
}


void TournamentTeam::write_stats_in_server_log() {
    ServerLogger::LogType header_type;
    ServerLogger::LogType detail_type_red;
    ServerLogger::LogType detail_type_blue;

    if (strcmp(tournament_type(), "TDM")) {
        header_type = ServerLogger::LogTypeStatsTDMTeamScore;
        detail_type_red = ServerLogger::LogTypeStatsTDMTeamRed;
        detail_type_blue = ServerLogger::LogTypeStatsTDMTeamBlue;
    } else if (strcmp(tournament_type(), "CTF")) {
        header_type = ServerLogger::LogTypeStatsCTFTeamScore;
        detail_type_red = ServerLogger::LogTypeStatsCTFTeamRed;
        detail_type_blue = ServerLogger::LogTypeStatsCTFTeamBlue;
    } else {
        header_type = ServerLogger::LogTypeStatsGOHTeamScore;
        detail_type_red = ServerLogger::LogTypeStatsGOHTeamRed;
        detail_type_blue = ServerLogger::LogTypeStatsGOHTeamBlue;
    }

    /* team score */
    logger->log(header_type, "team stat", 0, 0,
        team_red_name.c_str(), &score.score_red,
        team_blue_name.c_str(), &score.score_blue);

    /* player scores */
    for (int i = 0; i < 2; i++) {
        int flags = (!i ? PlayerServerFlagTeamRed : 0);
        ServerLogger::LogType detail_type = (!i ? detail_type_red : detail_type_blue);

        std::vector<Player *> ranking;

        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if ((p->state.server_state.flags & PlayerServerFlagTeamRed) == flags) {
                if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
                    ranking.push_back(p);
                }
            }
        }

        std::sort(ranking.begin(), ranking.end(), ComparePlayerScore);

        int rank = 0;
        sscore_t last_score;
        sscore_t current_score;
        for (std::vector<Player *>::iterator it = ranking.begin(); it != ranking.end(); it++) {
            Player *p = *it;
            current_score = p->state.server_state.score;
            if (rank) {
                if (current_score != last_score) {
                    rank++;
                }
            } else {
                rank++;
            }

            last_score = current_score;

            logger->log(detail_type, "player stat", p, 0, &rank, &current_score,
                &p->state.server_state.frags, &p->state.server_state.kills);
        }
    }
}