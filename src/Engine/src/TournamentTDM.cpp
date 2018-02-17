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

#include "TournamentTDM.hpp"

#include <cstdlib>

TournamentTDM::TournamentTDM(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    : TournamentTeam(resources, subsystem, gui, logger, game_file, server, map_name,
      players, duration, warmup)
{
    /* setup tournament icon */
    tournament_icon = resources.get_icon("hud_tdm");
}

TournamentTDM::~TournamentTDM() { }

const char *TournamentTDM::tournament_type() {
    return "TDM";
}

std::string TournamentTDM::get_game_type_name() const {
    return i18n(I18N_MAINMENU_GM_TDM);
}

void TournamentTDM::team_fire_point(Player *penem, Team team, int points) {
    if (team == TeamRed) {
        score.score_red += points;
        add_i18n_response(I18N_TNMT_TEAM_RED_SCORED1);
        if (logger) {
            logger->log(ServerLogger::LogTypeTeamRedScored, i18n(I18N_TNMT_TEAM_RED_SCORED1), penem);
        }
        add_team_score_animation(penem, I18N_TNMT_TEAM_RED_SCORED2);
    } else {
        score.score_blue += points;
        add_i18n_response(I18N_TNMT_TEAM_BLUE_SCORED1);
        if (logger) {
            logger->log(ServerLogger::LogTypeTeamBlueScored, i18n(I18N_TNMT_TEAM_BLUE_SCORED1), penem);
        }
        add_team_score_animation(penem, I18N_TNMT_TEAM_BLUE_SCORED2);
    }
    send_team_score();
}
