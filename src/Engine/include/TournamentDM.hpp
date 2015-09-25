#ifndef TOURNAMENTDM_HPP
#define TOURNAMENTDM_HPP

#include "Tournament.hpp"

/* pointing system:
 *
 * frag             1
 * see Tournament
 *
 */

class TournamentDM : public Tournament {
public:
    TournamentDM(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup)
        throw (TournamentException, ResourcesException);
    virtual ~TournamentDM();

    virtual const char *tournament_type();
    virtual void write_stats_in_server_log();

    virtual void frag_point(Player *pfrag, Player *pkill);
    virtual void draw_statistics();

private:
    int draw_stats(Font *f, int x, int y);
};

#endif
