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

#include "TournamentFactory.hpp"
#include "TournamentDM.hpp"
#include "TournamentTDM.hpp"
#include "TournamentCTF.hpp"
#include "TournamentSR.hpp"
#include "TournamentCTC.hpp"
#include "TournamentGOH.hpp"

#include <cstdlib>

static bool to_bool(const std::string& v) {
    return (atoi(v.c_str()) ? true : false);
}

TournamentFactory::TournamentFactory(Resources& resources, Subsystem& subsystem, Gui *gui)
    : resources(resources), subsystem(subsystem), gui(gui), tournament_id(0) { }

TournamentFactory::~TournamentFactory() { }

Tournament *TournamentFactory::create_tournament(const MapConfiguration& config,
    bool server, bool warmup, Players& players, ServerLogger *logger)
{
    int duration = (warmup ? config.warmup_in_seconds : config.duration * 60);
    Tournament *tournament = 0;

    switch (config.type) {
        case GamePlayTypeDM:
            tournament = new TournamentDM(resources, subsystem, gui, logger, "dm",
                server, config.map_name, players, duration, warmup);
            break;

        case GamePlayTypeTDM:
            tournament = new TournamentTDM(resources, subsystem, gui, logger, "tdm",
                server, config.map_name, players, duration, warmup);
            break;

        case GamePlayTypeCTF:
            tournament = new TournamentCTF(resources, subsystem, gui, logger, "ctf",
                server, config.map_name, players, duration, warmup);
            break;

        case GamePlayTypeSR:
            tournament = new TournamentSR(resources, subsystem, gui, logger, "sr",
                server, config.map_name, players, duration, warmup);
            break;

        case GamePlayTypeCTC:
            tournament = new TournamentCTC(resources, subsystem, gui, logger, "ctc",
                server, config.map_name, players, duration, warmup);
            break;

        case GamePlayTypeGOH:
            tournament = new TournamentGOH(resources, subsystem, gui, logger, "goh",
                server, config.map_name, players, duration, warmup);
            break;
    }

    if (tournament && server) {
        tournament_id++;
        if (logger) {
            logger->log(ServerLogger::LogTypeNewMap, subsystem.get_i18n()(I18N_TNMT_NEW_MAP_LOADED));
        }
    }

    return tournament;
}

unsigned char TournamentFactory::get_tournament_id() const {
    return tournament_id;
}

void TournamentFactory::set_tournament_id(unsigned char id) {
    tournament_id = id;
}

void TournamentFactory::set_tournament_server_flags(Properties& properties, Tournament *tournament) {
    if (tournament) {
        tournament->set_flag(Tournament::SettingEnableFriendlyFire, to_bool(properties.get_value("friendly_fire_alarm")));
        tournament->set_flag(Tournament::SettingEnableShotExplosives, to_bool(properties.get_value("shot_explosives")));
        tournament->set_flag(Tournament::SettingEnablePreventPick, to_bool(properties.get_value("prevent_pick")));
    }
}
