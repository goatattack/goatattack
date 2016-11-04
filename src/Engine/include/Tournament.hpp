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

#ifndef _TOURNAMENT_HPP_
#define _TOURNAMENT_HPP_

#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "KeyValue.hpp"
#include "Map.hpp"
#include "Tileset.hpp"
#include "Player.hpp"
#include "Timing.hpp"
#include "GameProtocol.hpp"
#include "Animation.hpp"
#include "Configuration.hpp"
#include "Gui.hpp"
#include "Globals.hpp"
#include "ServerLogger.hpp"
#include "I18N.hpp"
#include "Lagometer.hpp"

#include <vector>
#include <deque>

/* pointing system:
 *
 * suicide         -1
 *
 */

class TournamentException : public Exception {
public:
    TournamentException(const char *msg) : Exception(msg) { }
    TournamentException(const std::string& msg) : Exception(msg) { }
};

const double AnimationMultiplier = 3.0f;
const double TextAnimationSpeed = 5.0f;
const double XAccel = 0.055f;
const double XDecel = 0.01f;
const double XMaxAccel = 1.75f;
const double YAccelGravity = 0.075f;
const double YMaxAccel = 4.5f;
const double YVeloLanding = 2.25f;
const double YDecelJumpNormal = 0.075f;
const double YDecelJump = 0.005f;

struct GameAnimation {
    GameAnimation() : animation(0), animation_counter(0.0f),
        index(0), falling(false),
        last_falling_y_pos(Player::PlayerFallingTestMaxY),
        sound_channel(-1), delete_me(false) { }

    Animation *animation;
    GAnimationState state;
    double animation_counter;
    int index;
    bool falling;
    int last_falling_y_pos;
    int sound_channel;
    bool delete_me;
};

struct GameTextAnimation {
    GameTextAnimation() : animation_counter(0.0f), rise_counter(0),
        max_rise_counter(0), delete_me(false) { }

    Font *font;
    std::string text;
    int x;
    int y;
    double animation_counter;
    int rise_counter;
    int max_rise_counter;
    bool delete_me;
};

struct GameObject {
    GameObject() : picked(false), spawn_counter(0.0f),
        falling(false), last_falling_y_pos(Player::PlayerFallingTestMaxY),
        spawned_object(false), delete_me(false) { }

    Object *object;
    GObjectState state;
    int origin_x;
    int origin_y;
    bool picked;
    double spawn_counter;
    bool falling;
    int last_falling_y_pos;
    bool spawned_object;
    bool delete_me;
};

struct SpawnableNPC {
    SpawnableNPC() : npc(0), init_owner(0), move_counter(0.0f), in_idle(false),
        idle_counter(0.0f), idle_counter_init(0.0f), ignore_owner_counter(0.0f),
        falling(false), last_falling_y_pos(Player::PlayerFallingTestMaxY),
        delete_me(false) { }

    NPC *npc;
    GNPCState state;
    NPCAnimation icon;
    int iconindex;
    identifier_t init_owner;
    double move_counter;
    bool in_idle;
    double idle_counter;
    double idle_counter_init;
    double ignore_owner_counter;
    bool falling;
    int last_falling_y_pos;
    bool delete_me;
};

struct StateResponse {
    StateResponse(int action) : action(action), len(0), data(0) { }
    StateResponse(int action, data_len_t len, const data_t *data)
        : action(action), len(len), data(data) { }

    int action;
    data_len_t len;
    const data_t *data;
};

struct GenericData {
    GenericData(void *data, size_t sz) : data(data), sz(sz), next(0) { }

    void *data;
    size_t sz;
    GenericData *next;
};

class Tournament {
private:
    Tournament(const Tournament&);
    Tournament& operator=(const Tournament&);

public:
    typedef std::deque<StateResponse *> StateResponses;
    typedef std::vector<GameObject *> GameObjects;
    typedef std::vector<SpawnableNPC *> SpawnableNPCs;
    typedef std::vector<GameAnimation *> GameAnimations;

    enum Setting {
        SettingEnableFriendlyFire = 0,
        SettingEnablePreventPick,
        SettingEnableShotExplosives,
        _MAXSettings
    };

    Tournament(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
        const std::string& game_file, bool server,
        const std::string& map_name, Players& players, int duration, bool warmup)
        throw (TournamentException, ResourcesException);
    virtual ~Tournament();

    Map& get_map();
    Tileset *get_tileset();
    StateResponses& get_state_responses();
    GameObjects& get_game_objects();
    SpawnableNPCs& get_spawnable_npcs();
    GameAnimations& get_game_animations();
    GGameState& get_game_state();

    void set_gui_is_destroyed(bool state);
    void set_following_id(player_id_t id);
    void set_ping_time(ms_t ms);
    void set_player_configuration(Configuration *config);
    void set_ready();
    bool is_ready() const;
    bool is_game_over() const;
    void spectate_request();
    void spectate_accepted();
    void show_stats(bool state);
    void update_states(ns_t ns);
    void set_lagometer(Lagometer *lagometer);

    void add_animation(GAnimation *animation);
    void remove_animation(identifier_t id);
    void add_spawnable_npc(GSpawnNPC *npc);
    void add_npc_remove_animation(SpawnableNPC *npc);
    void remove_marked_npcs();
    void send_remove_npc(SpawnableNPC *npc);
    void remove_spawnable_npc(GRemoveNPC *rnpc);
    void add_text_animation(GTextAnimation *animation);
    void add_pick_object(GPickObject *po);
    void add_place_object(GPlaceObject *po);
    void spawn_object(GSpawnObject *so);
    void spawn_object(Object *obj, identifier_t id, int x, int y, flags_t flags);
    void add_player_spawn_animation(Player *p);
    void fire_shot(Player *p, unsigned char direction);
    void fire_grenade(Player *p, unsigned char direction);
    void fire_bomb(Player *p, unsigned char direction);
    void fire_frog(Player *p, unsigned char direction);
    void retrieve_flags();
    void set_flag(Setting setting, bool value);

    void draw();
    void delete_responses();
    identifier_t create_animation_id();

    static char *create_i18n_response(I18NText id, size_t& sz, const char *addon = 0);

    virtual const char *tournament_type() = 0;

    virtual void create_spawn_points() throw (TournamentException);
    virtual void player_added(Player *p);
    virtual void player_removed(Player *p);
    virtual void spawn_player(Player *p);
    virtual void add_team_score(GTeamScore *ts);
    virtual void retrieve_states();
    virtual bool player_joins(Player *p, playerflags_t flags);
    virtual void join_accepted();
    virtual void join_refused();
    virtual void update_wearable_remaining(GTimeRemaining *remain);
    virtual bool friendly_fire_alarm(GFriendyFireAlarm *alarm);
    virtual void players_post_actions();
    virtual void round_finished_set_time(Player *p, GTransportTime *race);
    virtual void score_transport_raw(void *data);
    virtual void reset_player(Player *p);
    virtual bool play_gun_error(Player *p);
    virtual bool play_grenade_error(Player *p);
    virtual bool play_bomb_error(Player *p);
    virtual bool play_frog_error(Player *p);
    virtual void reopen_join_window(Player *p);

    virtual GenericData *create_generic_data();
    virtual void destroy_generic_data(void *data);
    virtual void generic_data_delivery(void *data);

    void destroy_generic_data_list(GenericData *data);

protected:
    static const ns_t ns_fc = 10000000;   /* for 0.01 s */
    static const ns_t ns_sec = 1000000000;

    Resources& resources;
    Subsystem& subsystem;
    I18N& i18n;
    Properties& properties;
    Gui *gui;
    bool server;
    Map map;
    Tileset *tileset;
    short **map_array;
    short **decoration_array;
    Lightmap *lightmap;
    int map_width;
    int map_height;
    int tile_width;
    int tile_height;
    double decoration_brightness;
    Players& players;
    player_id_t following_id;
    Background *background;

    typedef std::vector<GameTextAnimation *> GameTextAnimations;
    typedef std::vector<GameObject *> SpawnPoints;
    typedef std::vector<GameObject *> FrogSpawnPoints;

    int top;
    int left;
    double spectator_x;
    double spectator_y;
    double spectator_accel_x;
    double spectator_accel_y;
    int parallax_shift;
    bool debug;
    GGameState game_state;
    ns_t second_counter;
    bool ready;
    ms_t ping_time;
    identifier_t animation_id;
    int screen_shaker;
    Animation *player_afk;
    double player_afk_counter;
    size_t player_afk_index;
    Configuration* player_configuration;
    bool show_statistics;
    Icon *screw1;
    Icon *screw2;
    Icon *screw3;
    Icon *screw4;
    bool last_button_a_state;
    bool warmup;
    Icon *tournament_icon;
    Icon *lives_full;
    Icon *lives_half;
    Icon *lives_empty;
    Icon *shield_full;
    Icon *shield_half;
    Icon *shield_empty;
    Icon *hud_ammo;
    Icon *hud_grenades;
    Icon *hud_bombs;
    Icon *hud_frogs;
    Icon *enemy_indicator;
    bool game_over;
    ServerLogger *logger;
    bool gui_is_destroyed;
    Lagometer *lagometer;

    int tilex;
    int tiley;
    StateResponses state_responses;
    GameAnimations game_animations;
    GameTextAnimations game_text_animations;
    GameObjects game_objects;
    SpawnPoints spawn_points;
    FrogSpawnPoints frog_spawn_points;
    SpawnableNPCs spawnable_npcs;
    bool settings[_MAXSettings];

    bool has_frogs;
    double frog_respawn_counter;
    int frog_spawn_init;

    enum TestType {
        TestTypeNormal,
        TestTypeFalling,
        TestTypeFallingThrough
    };

    void integrate(ns_t ns);

    bool collide_with_tile(TestType type, Player *p, int last_falling_y_pos,
        double x, double y, double *friction, bool *killing);

    void add_state_response(int action, data_len_t len, const void *data);
    void add_msg_response(const char *msg);
    void add_sound_response(const char *name);
    Player *get_following_player();
    Player *get_me();

    void add_animation(const std::string& name, identifier_t id,
        scounter_t duration, identifier_t owner, int x, int y,
        double accel_x, double accel_y, int width, int height);

    bool render_physics(double period_f, bool projectile, int damage, double recoil,
        identifier_t owner, double springiness_x, double springiness_y,
        const CollisionBox& colbox, double& x, double& y,
        double& accel_x, double& accel_y, int width, int height,
        double friction_factor, bool play_bump, bool& falling,
        int& last_falling_y_pos, bool *killing, const std::string& weapon);

    void play_ground_bump_sound();
    void check_attack(Player *p, const CollisionBox& colbox, bool move_up,
        bool move_down, Player *following_player);

    void firing_animation(Player *p, int flag, const std::string& animation_name,
        const std::string& start_sound, int yoffset, unsigned char direction,
        unsigned char& mun);

    void firing_npc(Player *p, int flag, const std::string& npc_name,
        const std::string& start_sound, int yoffset, unsigned char direction,
        unsigned char& mun);

    void check_killing_animation(int x, int y, Animation *ani,
        identifier_t owner, bool preserve_npc, NPC *npc);

    void player_damage(identifier_t owner, Player *p, NPC *npc, int damage, const std::string& weapon);

    void spawn_player_base(Player *p, SpawnPoints& spawn_points);

    void control_spectator(Player *me, double period_f);

    void draw_background();
    void draw_decoration();
    void draw_map(bool background);
    void draw_lightmaps();
    void draw_animations(bool background);
    void draw_objects();
    void draw_npcs();
    void draw_players();
    void draw_text_animations();
    void draw_player_names();
    void draw_hud();

    void player_dies(Player *p, I18NText id, const char *addon = 0);
    void join_handling();
    void reset_all_players_and_broadcast();
    identifier_t get_free_object_id();
    void reset_frog_spawn_counter();
    void spawn_frog();
    identifier_t get_free_npc_id();
    void update_npc_states(double period_f);
    void player_npc_collision(Player *p, SpawnableNPC *npc);

    void draw_lives_armor(int amount, Icon *full, Icon *half, Icon *empty, int y);

    void add_i18n_response(I18NText id, const char *addon = 0);
    void add_i18n_response(I18NText id, const std::string& p);
    void add_i18n_response(I18NText id, const std::string& p1, const std::string& p2);

    virtual void write_stats_in_server_log() = 0;
    virtual void subintegrate(ns_t ns);
    virtual void player_died(Player *p);
    virtual void draw_score();
    virtual void draw_object_addons();
    virtual void draw_player_addons();
    virtual void draw_team_colours();
    virtual void draw_statistics();
    virtual void draw_enemies_on_hud();
    virtual bool pick_item(Player *p, GameObject *obj);
    virtual bool tile_collision(TestType type, Player *p,
        int last_falling_y_pos, Tile *t, bool *killing);
    virtual bool fire_enabled(Player *p);
    virtual void frag_point(Player *pfrag, Player *pkill) = 0;
    virtual void player_join_request(Player *p);
    virtual void check_friendly_fire(identifier_t owner, Player *p);

    template <class T> static bool erase_element(T *elem) {
        if (elem->delete_me) {
            delete elem;
            return true;
        }

        return false;
    }
};

#endif
