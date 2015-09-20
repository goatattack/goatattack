#ifndef TOURNAMENTGOH_HPP
#define TOURNAMENTGOH_HPP

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * see TournamentTeam
 *
 */

class TournamentGOH : public TournamentTeam {
public:
    TournamentGOH(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup)
        throw (TournamentException, ResourcesException);
    virtual ~TournamentGOH();

    virtual const char *tournament_type();
    virtual void team_fire_point(Player *penem, Team team, int points);
    virtual void score_transport_raw(void *data);

private:
    Player *first_player_on_hill;
    Player *addon_player;
    double hill_counter;
    std::string crested_sound;
    std::string scored_sound;
    std::string leaved_sound;

    GHillCounter draw_hill_counter;

    virtual void subintegrate(ns_t ns);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual void draw_player_addons();
    virtual GenericData *create_generic_data();
    virtual void destroy_generic_data(void *data);
    virtual void generic_data_delivery(void *data);

    bool player_is_in_hill_zone(Player *p);
    void test_and_remove_player_from_hill(Player *p);
    void send_hill_counter(pos_t value);
    GHillCounter *create_hill_counter(pos_t value);
};

#endif