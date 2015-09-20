#include "TournamentFactory.hpp"
#include "TournamentDM.hpp"
#include "TournamentCTF.hpp"
#include "TournamentTDM.hpp"
#include "TournamentSR.hpp"
#include "TournamentCTC.hpp"
#include "TournamentGOH.hpp"

TournamentFactory::TournamentFactory(Resources& resources, Subsystem& subsystem, Gui *gui)
    throw (TournamentFactoryException)
    : resources(resources), subsystem(subsystem), gui(gui), tournament_id(0) { }

TournamentFactory::~TournamentFactory() { }

Tournament *TournamentFactory::create_tournament(const MapConfiguration& config,
    bool server, bool warmup, Players& players, ServerLogger *logger) throw (Exception)
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
            logger->log(ServerLogger::LogTypeNewMap, "");
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
