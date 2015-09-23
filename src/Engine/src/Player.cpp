#include "Player.hpp"

Player::Player(Resources& resources, const Connection *c, player_id_t player_id,
    const std::string& player_name, const std::string& characterset_name)
    : resources(resources), c(c), player_id(player_id), player_name(player_name),
      characterset(resources.get_characterset(characterset_name)),
      animation_counter(0.0f), font(0), player_name_width(0), its_me(false),
      respawning(false), joining(false), force_broadcast(false),
      flag_pick_refused_counter(0), flag_pick_refused(false), client_synced(false),
      server_force_quit_client(false), server_force_quit_client_counter(0),
      server_admin(false)
{
    reset();
}

Player::~Player() { }

const std::string& Player::get_player_name() const {
    return player_name;
}

void Player::set_player_name(const std::string& name) {
    player_name = name;
}

Characterset *Player::get_characterset() const {
    return characterset;
}

void Player::set_characterset(const std::string& name) throw (ResourcesException) {
    characterset = resources.get_characterset(name);
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

bool ComparePlayerScore(Player *lhs, Player *rhs) {
    return lhs->state.server_state.score > rhs->state.server_state.score;
}
