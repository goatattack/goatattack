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

#include "TournamentDM.hpp"

#include <algorithm>
#include <cstdlib>

TournamentDM::TournamentDM(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : Tournament(resources, subsystem, gui, logger, game_file, server, map_name, players,
      duration, warmup)
{
    /* create spawn points */
    create_spawn_points();

    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_dm");
}

TournamentDM::~TournamentDM() { }

const char *TournamentDM::tournament_type() {
    return "DM";
}

void TournamentDM::write_stats_in_server_log() {
    std::vector<Player *> ranking;

    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
            ranking.push_back(p);
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

        logger->log(ServerLogger::LogTypeStatsDM, i18n(I18N_TNMT_STATS_PLAYER), p, 0, &rank, &current_score,
            &p->state.server_state.frags, &p->state.server_state.kills);
    }
}

void TournamentDM::frag_point(Player *pfrag, Player *pkill) {
    pfrag->state.server_state.score += 1;
}

void TournamentDM::draw_statistics() {
    Font *font_normal = resources.get_font("normal");
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = (vw - 20) / 2;
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
    std::string txt(i18n(I18N_TNMT_SB_DM_TITLE));
    int tw = font_big->get_text_width(txt);
    subsystem.draw_text(font_big, vw / 2 - tw / 2, y + 18, txt);

    /* draw list */
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    y = 55;
    x = wx + 15;
    subsystem.draw_text(font_normal, x, y, "#");
    subsystem.draw_text(font_normal, x + 20, y, i18n(I18N_TNMT_SB_PLAYER));
    subsystem.draw_text(font_normal, x + 140, y, i18n(I18N_TNMT_SB_SCORE));
    subsystem.draw_text(font_normal, x + 180, y, i18n(I18N_TNMT_SB_FRAGS));
    subsystem.draw_text(font_normal, x + 220, y, i18n(I18N_TNMT_SB_KILLS));
    subsystem.draw_text(font_normal, x + 260, y, i18n(I18N_TNMT_SB_PING));
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

int TournamentDM::draw_stats(Font *f, int x, int y) {
    char buffer[16];
    std::vector<Player *> ranking;

    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
            ranking.push_back(p);
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
