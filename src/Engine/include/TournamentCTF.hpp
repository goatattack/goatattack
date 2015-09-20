#ifndef TOURNAMENTCTF_HPP
#define TOURNAMENTCTF_HPP

#include "TournamentTeam.hpp"

/* pointing system:
 *
 * capture flag     3
 * neutralize flag  2
 * and see TournamentTeam
 *
 */

class TournamentCTF : public TournamentTeam {
public:
    TournamentCTF(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup)
        throw (TournamentException, ResourcesException);
    virtual ~TournamentCTF();

    virtual const char *tournament_type();
    virtual void write_stats_in_server_log();
    virtual void update_wearable_remaining(GTimeRemaining *remain);

private:
    GameObject *red_flag;
    GameObject *blue_flag;

    virtual void subintegrate(ns_t ns);
    virtual void draw_object_addons();
    virtual void draw_player_addons();
    virtual bool pick_item(Player *p, GameObject *obj);

    virtual bool tile_collision(TestType type, Player *p, int last_falling_y_pos,
        Tile *t, bool *killing);

    virtual bool fire_enabled(Player *p);
    virtual void player_removed(Player *p);
    virtual void player_died(Player *p);
    virtual bool play_gun_error(Player *p);

    bool test_and_drop_flag(Player *p);
    void return_flag(GameObject *flag, int flags);
    void check_flag_validity(double period_f, const std::string& name, GameObject *flag);
    bool flag_not_at_origin(GameObject *flag);
    void send_flag_remaining(GameObject *flag);

    virtual void team_fire_point(Player *penem, Team team, int points);
};

#endif // TOURNAMENTCTF_HPP
