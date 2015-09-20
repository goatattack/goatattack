#include "TournamentTDM.hpp"

#include <cstdlib>

TournamentTDM::TournamentTDM(Resources& resources, Subsystem& subsystem, Gui *gui,
    ServerLogger *logger, const std::string& game_file, bool server,
    const std::string& map_name, Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
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

void TournamentTDM::team_fire_point(Player *penem, Team team, int points) {
    if (team == TeamRed) {
        score.score_red += points;
        std::string msg(team_red_name + " scores");
        add_msg_response(msg.c_str());
        if (logger) {
            logger->log(ServerLogger::LogTypeTeamRedScored, msg, penem);
        }
        std::string team_name = uppercase(team_red_name);
        add_team_score_animation(penem, team_name + " SCORES");
    } else {
        score.score_blue += points;
        std::string msg(team_blue_name + " scores");
        add_msg_response(msg.c_str());
        if (logger) {
            logger->log(ServerLogger::LogTypeTeamBlueScored, msg, penem);
        }
        std::string team_name = uppercase(team_blue_name);
        add_team_score_animation(penem, team_name + " SCORES");
    }
    send_team_score();
}
