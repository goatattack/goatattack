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

#include "TournamentGOH.hpp"

#include <cstdlib>

const int HillCounterMax = 750;

TournamentGOH::TournamentGOH(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : TournamentTeam(resources, subsystem, gui, logger, game_file, server, map_name,
      players, duration, warmup),
      first_player_on_hill(0), addon_player(0), hill_counter(0),
      crested_sound(properties.get_value("crested_sound")),
      scored_sound(properties.get_value("scored_sound")),
      leaved_sound(properties.get_value("leaved_sound"))
{
    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_goh");
}

TournamentGOH::~TournamentGOH() {
    memset(&draw_hill_counter, 0, sizeof(draw_hill_counter));
}

const char *TournamentGOH::tournament_type() {
    return "GOH";
}

void TournamentGOH::team_fire_point(Player *penem, Team team, int points) { }

void TournamentGOH::score_transport_raw(void *data) {
    GHillCounter *hc = reinterpret_cast<GHillCounter *>(data);
    hc->from_net();
    draw_hill_counter = *hc;

    addon_player = 0;
    if (draw_hill_counter.counter) {
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *ip = *it;
            if (ip->state.id == draw_hill_counter.id) {
                addon_player = ip;
                break;
            }
        }
    }
}

void TournamentGOH::subintegrate(ns_t ns) {
    TournamentTeam::subintegrate(ns);

    if (server) {
        double period_f = ns / static_cast<double>(ns_fc);
        Player *in_hill_zone = 0;
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if (p->is_alive_and_playing()) {
                if (player_is_in_hill_zone(p)) {
                    in_hill_zone = p;
                    if (p == first_player_on_hill) {
                        break;
                    }
                }
            }
        }
        if (!in_hill_zone) {
            if (first_player_on_hill) {
                send_hill_counter(0);
                add_sound_response(leaved_sound.c_str());
            }
            first_player_on_hill = 0;
        } else {
            if (in_hill_zone != first_player_on_hill) {
                first_player_on_hill = in_hill_zone;
                hill_counter = HillCounterMax;
                send_hill_counter(static_cast<pos_t>(hill_counter));
                add_sound_response(crested_sound.c_str());
            } else {
                hill_counter -= period_f;
                if (!(static_cast<int>(hill_counter) % 25)) {
                    send_hill_counter(static_cast<pos_t>(hill_counter));
                }

                if (hill_counter <= 0.0) {
                    send_hill_counter(0);
                    for (Players::iterator it = players.begin(); it != players.end(); it++) {
                        Player *p = *it;
                        if (p->is_alive_and_playing()) {
                            p->state.server_state.flags |= PlayerServerFlagDead;
                        }
                    }

                    if (first_player_on_hill->state.server_state.flags & PlayerServerFlagTeamRed) {
                        score.score_red++;
                        std::string team_name = uppercase(team_red_name);
                        add_team_score_animation(first_player_on_hill, I18N_TNMT_TEAM_RED_SCORED2);
                        add_i18n_response(I18N_TNMT_TEAM_RED_SCORED1);
                        if (logger) {
                            logger->log(ServerLogger::LogTypeTeamRedScored, i18n(I18N_TNMT_TEAM_RED_SCORED1), first_player_on_hill);
                        }
                    } else {
                        score.score_blue++;
                        std::string team_name = uppercase(team_blue_name);
                        add_team_score_animation(first_player_on_hill, I18N_TNMT_TEAM_BLUE_SCORED2);
                        add_i18n_response(I18N_TNMT_TEAM_BLUE_SCORED1);
                        if (logger) {
                            logger->log(ServerLogger::LogTypeTeamBlueScored, i18n(I18N_TNMT_TEAM_BLUE_SCORED1), first_player_on_hill);
                        }
                    }
                    add_sound_response(scored_sound.c_str());
                    send_team_score();
                    first_player_on_hill = 0;
                }
            }
        }
    }
}

void TournamentGOH::player_removed(Player *p) {
    test_and_remove_player_from_hill(p);
    Tournament::player_removed(p);
}

void TournamentGOH::player_died(Player *p) {
    test_and_remove_player_from_hill(p);
}

void TournamentGOH::draw_player_addons() {
    if (addon_player) {
        int x = static_cast<int>(addon_player->state.client_server_state.x);
        int y = static_cast<int>(addon_player->state.client_server_state.y) - 22;
        y -= Characterset::Height;
        int w = Characterset::Width;
        subsystem.set_color(0.0f, 0.0f, 0.0f, 1.0f);
        subsystem.draw_box(x + left, y + top, w, 5);
        subsystem.set_color(1.0f, 1.0f, 1.0f, 1.0f);
        w = static_cast<int>(w * (draw_hill_counter.counter /
            static_cast<double>(HillCounterMax)));
        subsystem.draw_box(x + left, y + top, w, 5);
        subsystem.reset_color();
    }
}

GenericData *TournamentGOH::create_generic_data() {
    GenericData *gd = 0;

    if (first_player_on_hill) {
        GHillCounter *hc = create_hill_counter(static_cast<pos_t>(hill_counter));
        gd = new GenericData(hc, GHillCounterLen);
    }

    return gd;
}

void TournamentGOH::destroy_generic_data(void *data) {
    GHillCounter *hc = reinterpret_cast<GHillCounter *>(data);
    delete hc;
}

void TournamentGOH::generic_data_delivery(void *data) {
    score_transport_raw(data);
}

bool TournamentGOH::player_is_in_hill_zone(Player *p) {
    static int tilex;
    static int tiley;

    CollisionBox colbox = Characterset::Colbox;
    colbox.x += static_cast<int>(p->state.client_server_state.x);
    colbox.y = static_cast<int>(p->state.client_server_state.y) - colbox.height - colbox.y;
    int width = colbox.width - 1;
    int height = colbox.height - 1;
    int y = 0;
    while (true) {
        int x = 0;
        while (true) {
            tilex = static_cast<int>(x + colbox.x) / tile_width;
            tiley = static_cast<int>(y + colbox.y) / tile_height;

            bool do_test = true;
            if (tilex < 0 || tiley < 0) {
                do_test = false;
            }
            if (tilex > map_width - 1 || tiley > map_height - 1) {
                do_test = false;
            }

            if (do_test) {
                int index = map_array[tiley][tilex];
                if (index > -1) {
                    Tile *t = tileset->get_tile(index);
                    if (t->get_tile_type() == Tile::TileTypeHillZone) {
                        return true;
                    }
                }
            }

            /* check end? */
            if (x == width) {
                break;
            }

            /* advance next y tile */
            x += tile_width;
            if (x > width) {
                x = width;
            }
        }

        /* check end? */
        if (y == height) {
            break;
        }

        /* advance next y tile */
        y += tile_height;
        if (y > height) {
            y = height;
        }
    }

    return false;
}

void TournamentGOH::test_and_remove_player_from_hill(Player *p) {
    if (server) {
        if (p == first_player_on_hill) {
            send_hill_counter(0);
            first_player_on_hill = 0;
        }
    }
}

void TournamentGOH::send_hill_counter(pos_t value) {
    if (first_player_on_hill) {
        GHillCounter *hc = create_hill_counter(value);
        add_state_response(GPCScoreTransportRaw, GHillCounterLen, hc);
    }
}

GHillCounter *TournamentGOH::create_hill_counter(pos_t value) {
    GHillCounter *hc = new GHillCounter;
    hc->id = first_player_on_hill->state.id;
    hc->counter = value;
    hc->to_net();

    return hc;
}