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

#include "Tournament.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

Tournament::Tournament(Resources& resources, Subsystem& subsystem, Gui *gui, ServerLogger *logger,
    const std::string& game_file, bool server, const std::string& map_name,
    Players& players, int duration, bool warmup)
    throw (TournamentException, ResourcesException)
    : resources(resources), subsystem(subsystem), i18n(subsystem.get_i18n()),
      properties(*resources.get_game_settings(game_file)),
      gui(gui), server(server),
      map(*resources.get_map(map_name)),
      tileset(resources.get_tileset(map.get_tileset())),
      map_array(map.get_map()), decoration_array(map.get_decoration()),
      lightmap(0), map_width(map.get_width()), map_height(map.get_height()),
      tile_width(tileset->get_tile_width()), tile_height(tileset->get_tile_height()),
      decoration_brightness(map.get_decoration_brightness()), players(players), following_id(0),
      background(resources.get_background(map.get_background())), top(0), left(0),
      spectator_x(map_width * tile_width / 2),
      spectator_y(map_height * tile_height / 2),
      spectator_accel_x(0), spectator_accel_y(0), parallax_shift(1), debug(false),
      game_state(duration), second_counter(0), ready(server),
      ping_time(0), animation_id(0), screen_shaker(0),
      player_afk(resources.get_animation("player_afk")), player_afk_counter(0),
      player_afk_index(0), player_configuration(0), show_statistics(false),
      screw1(resources.get_icon("screw1")), screw2(resources.get_icon("screw2")),
      screw3(resources.get_icon("screw1")), screw4(resources.get_icon("screw2")),
      last_button_a_state(false), warmup(warmup), tournament_icon(0),
      lives_full(resources.get_icon("lives")),
      lives_half(resources.get_icon("lives_half")),
      lives_empty(resources.get_icon("lives_empty")),
      shield_full(resources.get_icon("shield")),
      shield_half(resources.get_icon("shield_half")),
      shield_empty(resources.get_icon("shield_empty")),
      hud_ammo(resources.get_icon("ammo")),
      hud_grenades(resources.get_icon("grenade")),
      hud_bombs(resources.get_icon("bomb")),
      hud_frogs(resources.get_icon("frog")),
      enemy_indicator(resources.get_icon("enemy_indicator_neutral")),
      game_over(false), logger(logger), gui_is_destroyed(false),
      do_friendly_fire_alarm(true)
{
    /* init */
    char kvb[128];

    /* get parallax shift from map */
    int shift = map.get_parallax_shift();
    if (shift) {
        parallax_shift = shift;
    }

    /* create map objects */
    has_frogs = false;
    int objcount = atoi(map.get_value("objects").c_str());
    for (int i = 0; i < objcount; i++) {
        sprintf(kvb, "object_name%d", i);
        const std::string& objname = map.get_value(kvb);
        if (objname.length()) {
            Object *obj = resources.get_object(objname);
            GameObject *gobj = new GameObject;
            gobj->picked = !server;
            gobj->object = obj;
            gobj->state.id = static_cast<identifier_t>(i);
            sprintf(kvb, "object_x%d", i);
            gobj->state.x = atoi(map.get_value(kvb).c_str()) * tile_width;
            sprintf(kvb, "object_y%d", i);
            gobj->state.y = atoi(map.get_value(kvb).c_str()) * tile_height;
            gobj->state.accel_x = 0.0f;
            gobj->state.accel_y = 0.0f;
            gobj->origin_x = static_cast<int>(gobj->state.x);
            gobj->origin_y = static_cast<int>(gobj->state.y);
            game_objects.push_back(gobj);
            if (gobj->object->get_type() == Object::ObjectTypeSpawnPointFrog) {
                has_frogs = true;
                frog_spawn_points.push_back(gobj);
            }
        }
    }

    /* create lightmap */
    if (!server) {
        map.create_lightmap();
        lightmap = map.get_lightmap();
    }

    /* check if frog's spawn points are placed in map */
    if (has_frogs) {
        frog_spawn_init = map.get_frog_spawn_init();
        if (!frog_spawn_init) {
            frog_spawn_init = atoi(properties.get_value("frog_spawn_init").c_str());
        }
        if (frog_spawn_init < 1) {
            frog_spawn_init = 1200;
        }
        reset_frog_spawn_counter();
    }

    /* setup logger */
    if (logger) {
        logger->set_map(&map);
    }
}

Tournament::~Tournament() {
    delete_responses();

    for (GameAnimations::iterator it = game_animations.begin();
        it != game_animations.end(); it++)
    {
        delete *it;
    }

    for (GameTextAnimations::iterator it = game_text_animations.begin();
        it != game_text_animations.end(); it++)
    {
        delete *it;
    }

    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        delete *it;
    }

    for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
        it != spawnable_npcs.end(); it++)
    {
        delete *it;
    }

    if (logger) {
        logger->log(ServerLogger::LogTypeMapClosed, i18n(I18N_TNMT_STATS_MAP_CLOSED));
        logger->set_map(0);
    }
}

void Tournament::set_gui_is_destroyed(bool state) {
    gui_is_destroyed = state;
}

identifier_t Tournament::create_animation_id() {
    return ++animation_id;
}

Map& Tournament::get_map() {
    return map;
}

Tileset *Tournament::get_tileset() {
    return tileset;
}

Tournament::StateResponses& Tournament::get_state_responses() {
    return state_responses;
}

Tournament::GameObjects& Tournament::get_game_objects() {
    return game_objects;
}

Tournament::SpawnableNPCs& Tournament::get_spawnable_npcs() {
    return spawnable_npcs;
}

Tournament::GameAnimations& Tournament::get_game_animations() {
    return game_animations;
}

GGameState& Tournament::get_game_state() {
    return game_state;
}

void Tournament::set_following_id(player_id_t id) {
    following_id = id;
}

void Tournament::set_ping_time(ms_t ms) {
    ping_time = ms;
}

void Tournament::set_player_configuration(Configuration *config) {
    player_configuration = config;
}

void Tournament::set_ready() {
    ready = true;
}

bool Tournament::is_ready() const {
    return ready;
}

bool Tournament::is_game_over() const {
    return game_over;
}

void Tournament::spectate_request() {
    if (!server) {
        add_state_response(GPSSpectate, 0, 0);
    }
}

void Tournament::spectate_accepted() {
    if (!server) {
        Player *me = get_me();
        if (me) {
            me->reset();
        }
    }
}

void Tournament::show_stats(bool state) {
    show_statistics = ((!game_state.seconds_remaining && !warmup) ? true : state);
}

void Tournament::add_animation(GAnimation *animation) {
    GameAnimation *gani = 0;
    try {
        Animation *ani = resources.get_animation(animation->animation_name);

        int shaker = ani->get_screen_shaker();
        if (shaker) {
            screen_shaker = shaker;
        }

        int spread = ani->get_spread();
        int spread_count = ani->get_spread_count();
        int bx = 0;
        int by = 0;
        for (int i = 0; i < spread_count; i++) {
            if (i) {
                bx = rand() % spread - spread / 2;
                by = rand() % spread - spread / 2;
            }
            gani = new GameAnimation;
            gani->animation = ani;
            gani->state.id = animation->id;
            gani->state.duration = animation->duration;
            gani->state.owner = animation->owner;
            gani->state.x = animation->x + bx;
            gani->state.y = animation->y + by;
            gani->state.accel_x = animation->accel_x;
            gani->state.accel_y = animation->accel_y;
            gani->index = ani->get_randomized_index();
            game_animations.push_back(gani);
        }
        if (!server) {
            if (strlen(animation->sound_name)) {
                subsystem.play_sound(resources.get_sound(animation->sound_name), 0);
            }
        }
    } catch (const ResourcesException& e) {
        if (gani) delete gani;
        subsystem << i18n(I18N_ANIMATION_FAILED, e.what()) << std::endl;
    }
}

void Tournament::add_animation(const std::string& name, identifier_t id,
    scounter_t duration, identifier_t owner, int x, int y,
    double accel_x, double accel_y, int width, int height)
{
    GameAnimation *gani = 0;
    try {
        Animation *ani = resources.get_animation(name);
        TileGraphic *tg = ani->get_tile()->get_tilegraphic();
        gani = new GameAnimation;
        gani->animation = ani;
        gani->state.id = id; /* client side generated are 0 */
        gani->state.duration = duration;
        gani->state.owner = owner;
        gani->state.x = x + width / 2 - tg->get_width() / 2;
        gani->state.y = y  + height / 2 - tg->get_height() / 2;
        gani->state.accel_x = accel_x;
        gani->state.accel_y = accel_y;
        game_animations.push_back(gani);
        if (!server) {
            subsystem.play_sound(ani->get_sound(), ani->get_sound_loops());
        }
    } catch (const ResourcesException& e) {
        if (gani) delete gani;
        subsystem << i18n(I18N_ANIMATION_FAILED, e.what()) << std::endl;
    }
}

void Tournament::add_text_animation(GTextAnimation *animation) {
    GameTextAnimation *gani = 0;
    try {
        gani = new GameTextAnimation;
        gani->font = resources.get_font(animation->font_name);
        gani->text = i18n(static_cast<I18NText>(animation->i18n_id));
        gani->x = static_cast<int>(animation->x) - gani->font->get_text_width(gani->text) / 2;
        gani->y = static_cast<int>(animation->y);
        gani->max_rise_counter = animation->max_counter;
        game_text_animations.push_back(gani);
    } catch (const ResourcesException& e) {
        if (gani) delete gani;
        subsystem << i18n(I18N_ANIMATION_FAILED, e.what()) << std::endl;
    }
}

void Tournament::add_pick_object(GPickObject *po) {
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        if (obj->state.id == po->id) {
            obj->picked = true;
            obj->delete_me = obj->object->is_spawnable();
            const std::string& pickup_animation = obj->object->get_value("pickup_animation");
            if (pickup_animation.length()) {
                TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();
                add_animation(pickup_animation, 0, 0, 0, static_cast<int>(obj->state.x),
                    static_cast<int>(obj->state.y), 0.0f, 0.0f, tg->get_width(), tg->get_height());
            }
            if (!server) {
                const std::string& pickup_sound = obj->object->get_value("pickup_sound");
                if (pickup_sound.length()) {
                    Sound *sound = resources.get_sound(pickup_sound);
                    subsystem.play_sound(sound, 0);
                }
            }
            break;
        }
    }

    /* delete marked objects */
    game_objects.erase(std::remove_if(game_objects.begin(),
        game_objects.end(), erase_element<GameObject>),
        game_objects.end());
}

void Tournament::add_place_object(GPlaceObject *po) {
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        if (obj->state.id == po->id) {
            obj->picked = false;
            obj->state.x = po->x;
            obj->state.y = po->y;
            if (po->flags & PlaceObjectWithAnimation) {
                const std::string& spawn_animation = obj->object->get_value("spawn_animation");
                if (spawn_animation.length()) {
                    TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();
                    add_animation(spawn_animation, 0, 0, 0, static_cast<int>(obj->state.x),
                        static_cast<int>(obj->state.y), 0.0f, 0.0f, tg->get_width(), tg->get_height());
                }
            }
            if (po->flags & PlaceObjectResetVelocity) {
                obj->state.accel_x = 0;
                obj->state.accel_y = 0;
            }
            if (!server) {
                if (po->flags & PlaceObjectWithSpawnSound) {
                    const std::string& sound_name = obj->object->get_value("spawn_sound");
                    if (sound_name.length()) {
                        Sound *sound = resources.get_sound(sound_name);
                        subsystem.play_sound(sound, 0);
                    }
                }

                if (po->flags & PlaceObjectWithScoredSound) {
                    const std::string& sound_name = obj->object->get_value("score_sound");
                    if (sound_name.length()) {
                        Sound *sound = resources.get_sound(sound_name);
                        subsystem.play_sound(sound, 0);
                    }
                }

                if (po->flags & PlaceObjectWithDropSound) {
                    const std::string& sound_name = obj->object->get_value("drop_sound");
                    if (sound_name.length()) {
                        Sound *sound = resources.get_sound(sound_name);
                        subsystem.play_sound(sound, 0);
                    }
                }
            }

            break;
        }
    }
}

void Tournament::spawn_object(GSpawnObject *so) {
    try {
        Object *obj = resources.get_object(so->object_name);
        spawn_object(obj, so->id, static_cast<int>(so->x), static_cast<int>(so->y), so->flags);
    } catch (const Exception& e) {
        subsystem << e.what() << std::endl;
    }
}

void Tournament::spawn_object(Object *obj, identifier_t id, int x, int y, flags_t flags) {
    GameObject *nobj = new GameObject;
    nobj->object = obj;
    nobj->origin_x = x;
    nobj->origin_y = y;
    nobj->state.id = id;
    nobj->state.accel_x = 0.0f;
    nobj->state.accel_y = 0.0f;
    nobj->state.x = static_cast<double>(x);
    nobj->state.y = static_cast<double>(y);
    nobj->spawned_object = true;
    game_objects.push_back(nobj);

    if (!server) {
        if (flags & PlaceObjectWithAnimation) {
            const std::string& spawn_animation = obj->get_value("spawn_animation");
            if (spawn_animation.length()) {
                TileGraphic *tg = obj->get_tile()->get_tilegraphic();
                add_animation(spawn_animation, 0, 0, 0, x, y, 0.0f, 0.0f,
                    tg->get_width(), tg->get_height());
            }
        }

        if (flags & PlaceObjectWithSpawnSound) {
            const std::string& sound_name = obj->get_value("spawn_sound");
            if (sound_name.length()) {
                Sound *sound = resources.get_sound(sound_name);
                subsystem.play_sound(sound, 0);
            }
        }
    }
}

void Tournament::add_player_spawn_animation(Player *p) {
    Animation *ani = resources.get_animation(Characterset::SpawnAnimation);
    TileGraphic *tg = ani->get_tile()->get_tilegraphic();
    int x = static_cast<int>(p->state.client_server_state.x);
    int y = static_cast<int>(p->state.client_server_state.y) - Characterset::Height;
    add_animation(Characterset::SpawnAnimation, 0, 0, 0, x, y, 0.0f, 0.0f, tg->get_width(), tg->get_height());
    subsystem.play_sound(resources.get_sound("respawn"), 0);
}

void Tournament::add_state_response(int action, data_len_t len, const void *data) {
    state_responses.push_back(new StateResponse(action, len, reinterpret_cast<const data_t *>(data)));
}

void Tournament::add_msg_response(const char *msg) {
    size_t sz = strlen(msg);
    char *msgb = new char[sz];
    memcpy(msgb, msg, sz);
    add_state_response(GPCTextMessage, static_cast<data_len_t>(sz), msgb);
}

void Tournament::add_sound_response(const char *name) {
    GGenericName *gnam = new GGenericName;
    memset(gnam, 0, GGenericNameLen);
    strncpy(gnam->name, name, NameLength - 1);
    add_state_response(GPCPlaySound, GGenericNameLen, gnam);
}

Player *Tournament::get_following_player() {
    if (following_id) {
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if (p->state.id == following_id) {
                return p;
            }
        }
    }

    return 0;
}

Player *Tournament::get_me() {
    if (following_id) {
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if (p->marked_as_me()) {
                return p;
            }
        }
    }

    return 0;
}

void Tournament::delete_responses() {
    size_t sz = state_responses.size();
    for (size_t i = 0; i < sz; i++) {
        StateResponse *resp = state_responses[i];
        if (resp->data) {
            if (resp->action == GPCI18NText || resp->action == GPCTextMessage) {
                delete[] resp->data;
            } else {
                delete resp->data;
            }
        }
        delete resp;
    }
    state_responses.clear();
}

void Tournament::create_spawn_points() throw (TournamentException) {
    for (GameObjects::iterator it = game_objects.begin(); it != game_objects.end(); it++) {
        GameObject *obj = *it;
        Object::ObjectType type = obj->object->get_type();
        if (type == Object::ObjectTypeSpawnPointRed || type == Object::ObjectTypeSpawnPointBlue) {
            spawn_points.push_back(obj);
        }
    }
    if (!spawn_points.size()) {
        throw TournamentException(i18n(I18N_MISSING_SPAWN_POINTS));
    }
}

void Tournament::player_added(Player *p) { }

void Tournament::player_removed(Player *p) {
    for (GameAnimations::iterator it = game_animations.begin();
        it != game_animations.end(); it++)
    {
        GameAnimation *gani = *it;
        if (gani->state.owner == p->state.id) {
            gani->state.owner = 0;
        }
    }
}

void Tournament::spawn_player(Player *p) {
    spawn_player_base(p, spawn_points);
}

void Tournament::spawn_player_base(Player *p, SpawnPoints& spawn_points) {
    // TODO: better selection of spawn points, maybe order by last spawn point usage
    GameObject *obj = spawn_points[rand() % spawn_points.size()];
    TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();
    int w = tg->get_width();
    int h = tg->get_height();

    int x = static_cast<int>(obj->state.x);
    int y = static_cast<int>(obj->state.y);
    x += w / 2;
    y += h;
    x = x - Characterset::Colbox.width / 2 - Characterset::Colbox.x;
    p->spawn(x, y);
}

void Tournament::add_team_score(GTeamScore *ts) { }

void Tournament::retrieve_states() { }

void Tournament::player_died(Player *p) { }

void Tournament::score_transport_raw(void *data) { }

void Tournament::reset_player(Player *p) { }

bool Tournament::play_gun_error(Player *p) {
    return true;
}

bool Tournament::play_grenade_error(Player *p) {
    return true;
}

bool Tournament::play_bomb_error(Player *p) {
    return true;
}

bool Tournament::play_frog_error(Player *p) {
    return true;
}

void Tournament::reopen_join_window(Player *p) { }

bool Tournament::pick_item(Player *p, GameObject *obj) {
    switch (obj->object->get_type()) {
        case Object::ObjectTypeBomb:
            /* pick bomb */
            p->state.server_state.bombs++;
            if (p->state.server_state.bombs > 100) {
                p->state.server_state.bombs = 100;
            }
            break;

        case Object::ObjectTypeFrog:
            reset_frog_spawn_counter();
            p->state.server_state.frogs++;
            if (p->state.server_state.frogs > 100) {
                p->state.server_state.frogs = 100;
            }
            break;

        case Object::ObjectTypeRedFlag:
        case Object::ObjectTypeBlueFlag:
            /* flags can't be picked in dm mode */
            return false;

        case Object::ObjectTypeArmor:
            /* pick armor */
            p->state.server_state.armor += 25;
            if (p->state.server_state.armor > 100) {
                p->state.server_state.armor = 100;
            }
            break;

        case Object::ObjectTypePoints:
            /* pick points */
            break;

        case Object::ObjectTypeGrenade:
            /* pick grenade */
            p->state.server_state.grenades += 5;
            if (p->state.server_state.grenades > 100) {
                p->state.server_state.grenades = 100;
            }
            break;

        case Object::ObjectTypeMedikitBig:
            /* pick big medikit */
            p->state.server_state.health += 100;
            if (p->state.server_state.health > 100) {
                p->state.server_state.health = 100;
            }
            break;

        case Object::ObjectTypeMedikitSmall:
            /* pick small medikit */
            p->state.server_state.health += 25;
            if (p->state.server_state.health > 100) {
                p->state.server_state.health = 100;
            }
            break;

        case Object::ObjectTypeShotgun:
            /* pick shotgun belt */
            if (!(p->state.server_state.flags & PlayerServerFlagHasShotgunBelt)) {
                p->state.server_state.flags |= PlayerServerFlagHasShotgunBelt;
            } else {
                return false;
            }
            break;

        case Object::ObjectTypeAmmo:
            /* pick ammo */
            if (p->state.server_state.flags & PlayerServerFlagHasShotgunBelt) {
                p->state.server_state.ammo += 10;
                if (p->state.server_state.ammo > 100) {
                    p->state.server_state.ammo = 100;
                }
            } else {
                return false;
            }
            break;

        case Object::ObjectTypeAmmobox:
            /* pick ammo */
            if (p->state.server_state.flags & PlayerServerFlagHasShotgunBelt) {
                p->state.server_state.ammo += 40;
                if (p->state.server_state.ammo > 100) {
                    p->state.server_state.ammo = 100;
                }
            } else {
                return false;
            }
            break;

        default:
            return false;
    }

    return true;
}

bool Tournament::fire_enabled(Player *p) {
    return true;
}

void Tournament::play_ground_bump_sound() {
    if (!server) {
        Sound *sound = resources.get_sound(properties.get_value("ground_bump_sound"));
        subsystem.play_sound(sound, 0);
    }
}

void Tournament::player_damage(identifier_t owner, Player *p, NPC *npc, int damage,
    const std::string& weapon)
{
    if (damage) {
        check_friendly_fire(owner, p);

        /* get damage levels */
        int damage2 = damage / (p->state.server_state.armor ? 2 : 1);

        /* reduce armor */
        if (damage >= p->state.server_state.armor) {
            p->state.server_state.armor = 0;
        } else {
            p->state.server_state.armor -= damage;
        }

        /* reduce health */
        if (damage2 >= p->state.server_state.health) {
            /* player dies */
            player_dies(p, I18N_NONE);

            /* increment owner's frag counter */
            I18NText frag_both = (npc ? I18N_TNMT_FROGGED1 : I18N_TNMT_KILLED1);
            I18NText frag_himself = (npc ? I18N_TNMT_FROGGED2 : I18N_TNMT_KILLED2);
            for (Players::iterator it = players.begin(); it != players.end(); it++) {
                Player *fp = *it;
                if (fp->state.id == owner) {
                    if (fp != p) {
                        fp->state.server_state.frags++;
                        frag_point(fp, p);
                        add_i18n_response(frag_both, fp->get_player_name(), p->get_player_name());
                        if (logger) {
                            logger->log(ServerLogger::LogTypeFrag, i18n(frag_both, fp->get_player_name(), p->get_player_name()), fp, p,
                                (npc ? npc->get_name().c_str() : weapon.c_str()));
                        }
                    } else {
                        fp->state.server_state.score -= 1;
                        add_i18n_response(frag_himself, fp->get_player_name());
                        if (logger) {
                            logger->log(ServerLogger::LogTypeKill, i18n(frag_himself, fp->get_player_name()), fp, p,
                                (npc ? npc->get_name().c_str() : weapon.c_str()));
                        }
                    }
                    break;
                }
            }
        } else {
            add_state_response(GPCPlayerHurt, 0, 0);
            p->state.server_state.health -= damage2;
        }
    }
}

void Tournament::player_dies(Player *p, I18NText id, const char *addon) {
    player_died(p);
    p->state.server_state.health = 0;
    p->state.server_state.flags |= PlayerServerFlagDead;
    p->state.server_state.kills++;
    if (id != I18N_NONE) {
        add_i18n_response(id, addon);
    }
    try {
        Animation *tempani = resources.get_animation(properties.get_value("die_animation"));

        GAnimation *ani = new GAnimation;
        memset(ani, 0, sizeof(GAnimation));
        strncpy(ani->animation_name, tempani->get_name().c_str(), NameLength - 1);
        strncpy(ani->sound_name, properties.get_value("die_sound").c_str(), NameLength - 1);

        TileGraphic *tga = tempani->get_tile()->get_tilegraphic();

        int x = static_cast<int>(p->state.client_server_state.x) + Characterset::Width / 2 - tga->get_width() / 2;
        int y = static_cast<int>(p->state.client_server_state.y) - Characterset::Height / 2 - tga->get_height() / 2;

        ani->id = ++animation_id;
        ani->x = x;
        ani->y = y;
        ani->to_net();
        add_state_response(GPCAddAnimation, sizeof(GAnimation), ani);
    } catch (const Exception& e) {
        subsystem << e.what() << std::endl;
    }
}

void Tournament::player_join_request(Player *p) {
    if (!p->joining) {
        p->joining = true;
        Player *me = get_me();
        if (me) {
            this->spawn_player(me);
            playerflags_t *flags = new playerflags_t;
            *flags = 0;
            add_state_response(GPSJoinRequest, sizeof(playerflags_t), flags);
        }
    }
}

bool Tournament::player_joins(Player *p, playerflags_t flags) {
    add_i18n_response(I18N_TNMT_PLAYER_JOINS, p->get_player_name());
    if (logger) {
        logger->log(ServerLogger::LogTypeJoin, i18n(I18N_TNMT_PLAYER_JOINS, p->get_player_name()), p);
    }

    return true;
}


void Tournament::join_accepted() {
    join_handling();
}

void Tournament::join_refused() {
    join_handling();
}

void Tournament::join_handling() {
    Player *me = get_me();
    if (me) {
        me->joining = false;
    }
}

void Tournament::update_wearable_remaining(GTimeRemaining *remain) { }

void Tournament::check_friendly_fire(identifier_t owner, Player *p) { }

bool Tournament::friendly_fire_alarm(GFriendyFireAlarm *alarm) {
    return false;
}

void Tournament::set_team_names(const std::string& team_red, const std::string& team_blue) {
    team_red_name = team_red;
    team_blue_name = team_blue;
}

void Tournament::destroy_generic_data_list(GenericData *data) {
    GenericData *s = data;
    GenericData *next = 0;
    while (s) {
        destroy_generic_data(s->data);
        next = s->next;
        delete s;
        s = next;
    }
}

GenericData *Tournament::create_generic_data() {
    return 0;
}

void Tournament::destroy_generic_data(void *data) { }

void Tournament::generic_data_delivery(void *data) { }

void Tournament::set_friendly_fire_alarm(bool state) {
    do_friendly_fire_alarm = state;
}

char *Tournament::create_i18n_response(I18NText id, size_t& sz, const char *addon) {
    sz = (addon ? strlen(addon) : 0);
    char *p = new char[GI18NTextLen + sz];
    GI18NText *t = reinterpret_cast<GI18NText *>(p);
    t->id = static_cast<identifier_t>(id);
    t->len = sz;
    if (sz) {
        memcpy(t->data, addon, sz);
    }
    t->to_net();

    sz += GI18NTextLen;

    return p;
}

void Tournament::add_i18n_response(I18NText id, const char *addon) {
    size_t sz;
    char *t = create_i18n_response(id, sz, addon);
    add_state_response(GPCI18NText, static_cast<data_len_t>(sz), t);
}

void Tournament::add_i18n_response(I18NText id, const std::string& p) {
    add_i18n_response(id, p.c_str());
}

void Tournament::add_i18n_response(I18NText id, const std::string& p1, const std::string& p2) {
    add_i18n_response(id, (p1 + "\t" + p2).c_str());
}