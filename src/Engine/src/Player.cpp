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

#include "Player.hpp"
#include "UTF8.hpp"

namespace {

    const char *CharactersetFallback = "goat";

}

Player::Player(Resources& resources, const Connection *c, player_id_t player_id,
    const std::string& player_name, const std::string& characterset_name)
    : resources(resources), c(c), player_id(player_id),
      player_name(utf8_validate(player_name.c_str())),
      fallback_characterset(get_characterset(CharactersetFallback)),
      characterset(get_characterset(characterset_name)), org_characterset(characterset),
      check_characterset(true), characterset_name(characterset_name),
      animation_counter(0.0f), font(0), player_name_width(0), its_me(false),
      respawning(false), joining(false), force_broadcast(false),
      flag_pick_refused_counter(0), flag_pick_refused(false), client_synced(false),
      server_force_quit_client(false), server_force_quit_client_counter(0),
      server_admin(false)
{
    reset();
}

const std::string& Player::get_player_name() const {
    return player_name;
}

void Player::set_player_name(const std::string& name) {
    player_name = utf8_validate(name.c_str());
}

Characterset *Player::get_characterset() const {
    if (check_characterset) {
        characterset = get_characterset(characterset_name);
        if (characterset != org_characterset) {
            check_characterset = false;
        }
    }
    return characterset;
}

const std::string& Player::get_characterset_name() const {
    return characterset_name;
}

void Player::set_characterset(const std::string& name) {
    characterset = get_characterset(name);
    characterset_name = name;
}

const Connection *Player::get_connection() const {
    return c;
}

void Player::mark_as_me() {
    its_me = true;
}

bool Player::marked_as_me() const {
    return its_me;
}

void Player::spawn(int x, int y) {
    clean_states();
    state.client_server_state.x = static_cast<double>(x);
    state.client_server_state.y = static_cast<double>(y);
}

bool Player::is_alive_and_playing() const {
    return (!(state.server_state.flags & (PlayerServerFlagDead | PlayerServerFlagSpectating)));
}

void Player::reset() {
    memset(&state, 0, sizeof(GPlayerState));
    state.id = player_id;
    reset_states();
    state.server_state.flags |= PlayerServerFlagSpectating;
}

void Player::clear() {
    reset_states();
    state.server_state.flags |= PlayerServerFlagDead;
}

void Player::zero() {
    clean_states();
}

void Player::set_to_default_characterset() {
    set_characterset(fallback_characterset->get_name());
}

void Player::clean_states() {
    state.server_state.health = 100;
    state.server_state.armor = 0;
    state.server_state.ammo = 0;
    state.server_state.bombs = 0;
    state.server_state.grenades = 0;
    state.server_state.frogs = 0;
    state.client_server_state.accel_x = 0.0f;
    state.client_server_state.accel_y = 0.0f;
    state.client_server_state.jump_accel_y = 0.0f;
    last_falling_y_pos = PlayerFallingTestMaxY;
    flag_pick_refused_counter = 0.0;
    flag_pick_refused = false;
}

void Player::reset_states() {
    state.server_state.flags &= (PlayerServerFlagTeamRed | PlayerServerFlagSpectating);
    clean_states();
    state.server_state.score = 0;
    state.server_state.frags = 0;
    state.server_state.kills = 0;
}

Characterset *Player::get_characterset(const std::string& name) const throw () {
    Characterset *cs = fallback_characterset;
    try {
        cs = resources.get_characterset(name);
    } catch (...) {
        /* chomp */
    }

    return cs;
}

bool ComparePlayerScore(Player *lhs, Player *rhs) {
    return lhs->state.server_state.score > rhs->state.server_state.score;
}
