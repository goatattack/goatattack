#ifndef TOURNAMENTTDM_HPP
#define TOURNAMENTTDM_HPP

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * see TournamentTeam
 *
 */

class TournamentTDM : public TournamentTeam {
public:
    TournamentTDM(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup)
        throw (TournamentException, ResourcesException);
    virtual ~TournamentTDM();

    virtual const char *tournament_type();
    virtual void team_fire_point(Player *penem, Team team, int points);
};

#endif
