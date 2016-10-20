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

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "MessageSequencer.hpp"
#include "GameProtocol.hpp"
#include "Resources.hpp"

#include <string>
#include <vector>

class Player {
private:
    Player(const Player&);
    Player& operator=(const Player&);

public:
    static const int PlayerFallingTestMaxY = 999999999;

    Player(Resources& resources, const Connection *c, player_id_t player_id,
        const std::string& player_name, const std::string& characterset_name);
    virtual ~Player();

    const std::string& get_player_name() const;
    void set_player_name(const std::string& name);
    Characterset *get_characterset() const;
    void set_characterset(const std::string& name) throw (ResourcesException);
    const Connection *get_connection() const;
    void mark_as_me();
    bool marked_as_me() const;
    void spawn(int x, int y);
    bool is_alive_and_playing() const;
    void clear();
    void reset();
    void zero();

private:
    Resources& resources;
    const Connection *c;
    player_id_t player_id;
    std::string player_name;
    Characterset *fallback_characterset;
    mutable Characterset *characterset;
    mutable Characterset *org_characterset;
    mutable bool check_characterset;
    std::string characterset_name;

    void clean_states();
    void reset_states();
    Characterset *get_characterset(const std::string& name) const throw ();

public:
    /* public accessors, without getters and setters */
    GPlayerState state;
    double animation_counter;
    Font *font;
    int player_name_width;
    bool its_me;
    bool respawning;
    bool joining;
    bool force_broadcast;
    double flag_pick_refused_counter;
    bool flag_pick_refused;
    bool client_synced;
    bool server_force_quit_client;
    int server_force_quit_client_counter;
    bool server_admin;

    int last_falling_y_pos;
};

typedef std::vector<Player *> Players;

bool ComparePlayerScore(Player *lhs, Player *rhs);

#endif
