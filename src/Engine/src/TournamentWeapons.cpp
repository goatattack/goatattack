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

#include <cstdlib>
#include <cmath>

void Tournament::check_attack(Player *p, const CollisionBox& colbox,
    bool move_up, bool move_down, Player *following_player)
{
    if (!server) {
        bool move_fire = ((p->state.client_server_state.key_states & PlayerKeyStateFire) != 0);
        bool move_grenade = ((p->state.client_server_state.key_states & PlayerKeyStateDrop1) != 0);
        bool move_bomb = ((p->state.client_server_state.key_states & PlayerKeyStateDrop2) != 0);
        bool move_frog = ((p->state.client_server_state.key_states & PlayerKeyStateDrop3) != 0);

        /* fire? */
        for (int weapon = 0; weapon < 4; weapon++) {
            int flag = 0;
            int flag2 = 0;
            int flag2ToDelete = 0;
            bool reset_animation_counter = false;
            bool move_key;
            bool is_fire_enabled;
            GPS action;
            double recoil = 0;
            unsigned char *mun = 0;
            bool play_weapon_error = true;

            switch (weapon) {
                case 0:
                    move_key = move_fire;
                    flag = PlayerClientFlagFireReleased;

                    if (!p->get_characterset()->get_suppress_shot_animation()) {
                        flag2 = PlayerClientFlagDoShotAnimation;
                        flag2ToDelete = PlayerClientFlagOneShotFinished;
                        reset_animation_counter = true;
                    }

                    action = GPSShot;
                    mun = &p->state.server_state.ammo;
                    if (p->state.server_state.flags & PlayerServerFlagHasShotgunBelt) {
                        is_fire_enabled = fire_enabled(p);
                    } else {
                        is_fire_enabled = false;
                    }
                    play_weapon_error = play_gun_error(p);
                    recoil = atof(properties.get_value("shot_recoil").c_str());
                    break;

                case 1:
                    move_key = move_grenade;
                    flag = PlayerClientFlagGrenadeReleased;
                    action = GPSGrenade;
                    mun = &p->state.server_state.grenades;
                    is_fire_enabled = fire_enabled(p);
                    play_weapon_error = play_grenade_error(p);
                    recoil = atof(properties.get_value("grenade_recoil").c_str());
                    break;

                case 2:
                    move_key = move_bomb;
                    flag = PlayerClientFlagBombReleased;
                    action = GPSBomb;
                    mun = &p->state.server_state.bombs;
                    is_fire_enabled = fire_enabled(p);
                    play_weapon_error = play_bomb_error(p);
                    recoil = atof(properties.get_value("bomb_recoil").c_str());
                    break;

                case 3:
                    move_key = move_frog;
                    flag = PlayerClientFlagFrogReleased;
                    action = GPSFrog;
                    mun = &p->state.server_state.frogs;
                    is_fire_enabled = fire_enabled(p);
                    play_weapon_error = play_frog_error(p);
                    recoil = atof(properties.get_value("frog_recoil").c_str());
                    break;
            }

            if (!move_key) {
                p->state.client_state.flags |= flag;
            } else {
                if (p->state.client_state.flags & flag) {
                    p->state.client_state.flags &= ~flag;
                    if (is_fire_enabled && (*mun) > 0) {
                        unsigned char *dir = new unsigned char;
                        *dir = p->state.client_server_state.direction;
                        add_state_response(action, sizeof(dir), dir);
                        p->state.client_state.flags |= flag2;
                        p->state.client_state.flags &= ~flag2ToDelete;
                        if (reset_animation_counter) {
                            p->animation_counter = 0.0f;
                            p->state.client_state.iconindex = 0;
                        }

                        if (p->state.client_server_state.direction == DirectionRight) {
                            p->state.client_server_state.accel_x += recoil;
                        } else {
                            p->state.client_server_state.accel_x -= recoil;
                        }
                    } else {
                        if (p == following_player) {
                            if (play_weapon_error) {
                                Sound *sound = resources.get_sound(properties.get_value("error_sound"));
                                if (!subsystem.is_sound_playing(sound)) {
                                    subsystem.play_controlled_sound(sound, 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Tournament::fire_shot(Player *p, unsigned char direction) {
    firing_animation(p, PlayerClientFlagFireReleased, properties.get_value("shot"),
        "shot_start_sound", atoi(properties.get_value("shot_y_offset").c_str()), direction,
        &p->state.server_state.ammo);
}

void Tournament::fire_grenade(Player *p, unsigned char direction) {
    firing_animation(p, PlayerClientFlagGrenadeReleased, properties.get_value("grenade"),
        "grenade_start_sound", atoi(properties.get_value("grenade_y_offset").c_str()), direction,
        &p->state.server_state.grenades);
}

void Tournament::fire_bomb(Player *p, unsigned char direction) {
    firing_animation(p, PlayerClientFlagBombReleased, properties.get_value("bomb"),
        "bomb_start_sound", atoi(properties.get_value("bomb_y_offset").c_str()), direction,
        &p->state.server_state.bombs);
}

void Tournament::fire_frog(Player *p, unsigned char direction) {
    firing_npc(p, PlayerClientFlagFrogReleased, properties.get_value("frog"),
        "frog_start_sound", atoi(properties.get_value("frog_y_offset").c_str()), direction,
        &p->state.server_state.frogs);
}

void Tournament::firing_animation(Player *p, int flag, const std::string& animation_name,
    const std::string& start_sound, int yoffset, unsigned char direction, unsigned char *mun)
{
    (*mun)--;
    const CollisionBox& colbox = p->get_characterset()->get_colbox();
    Animation *animation = resources.get_animation(animation_name);
    const CollisionBox& a_colbox = animation->get_physics_colbox();
    int ah = animation->get_tile()->get_tilegraphic()->get_height();

    GAnimation *ani = new GAnimation;
    memset(ani, 0, sizeof(GAnimation));
    strncpy(ani->animation_name, animation->get_name().c_str(), NameLength - 1);
    strncpy(ani->sound_name, properties.get_value(start_sound).c_str(), NameLength - 1);
    ani->id = ++animation_id;
    ani->duration = animation->get_duration();
    ani->owner = p->state.id;
    ani->x = p->state.client_server_state.x + colbox.x + (colbox.width / 2) - a_colbox.x - (a_colbox.width / 2);
    ani->y = p->state.client_server_state.y - colbox.y - (colbox.height / 2) - ah + (a_colbox.height / 2) + yoffset;
    double impact = animation->get_impact();
    if (animation->is_projectile()) {
        ani->y += static_cast<double>(p->get_characterset()->get_projectile_y_offset());
        if (direction == DirectionRight) {
            ani->accel_x = impact;
        } else {
            ani->accel_x = -impact;
        }
        ani->accel_y = 0.0f;
    } else {
        if (direction == DirectionRight) {
            ani->accel_x = fabs(p->state.client_server_state.accel_x) * impact;
        } else {
            ani->accel_x = fabs(p->state.client_server_state.accel_x) * -impact;
        }
        ani->accel_y = (p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y) * impact;
    }
    add_animation(ani);
    ani->to_net();
    add_state_response(GPCAddAnimation, sizeof(GAnimation), ani);
}

void Tournament::firing_npc(Player *p, int flag, const std::string& npc_name,
    const std::string& start_sound, int yoffset, unsigned char direction, unsigned char *mun)
{
    (*mun)--;
    NPC *npc = resources.get_npc(npc_name);
    GSpawnNPC *snpc = new GSpawnNPC;
    memset(snpc, 0, sizeof(GSpawnNPC));
    strncpy(snpc->npc_name, npc_name.c_str(), NameLength - 1);
    strncpy(snpc->sound_name, properties.get_value(start_sound).c_str(), NameLength - 1);
    snpc->id = get_free_npc_id();
    snpc->owner = p->state.id;
    snpc->direction = direction;
    snpc->icon = static_cast<unsigned char>(NPCAnimationStanding);
    snpc->x = p->state.client_server_state.x;
    snpc->y = p->state.client_server_state.y;
    double impact = npc->get_impact();
    if (direction == DirectionRight) {
        snpc->accel_x = fabs(p->state.client_server_state.accel_x) * impact;
    } else {
        snpc->accel_x = fabs(p->state.client_server_state.accel_x) * -impact;
    }
    snpc->accel_y = (p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y) * impact;
    add_spawnable_npc(snpc);
    snpc->to_net();
    add_state_response(GPCSpawnNPC, sizeof(GSpawnNPC), snpc);
}

void Tournament::check_killing_animation(int x, int y, Animation *ani,
    identifier_t owner, bool preserve_npc, NPC *npc)
{
    if (server) {
        if (ani->get_damage()) {
            int spread = ani->get_spread();

            CollisionBox colbox;
            colbox.x = x- spread / 2;
            colbox.y = y - spread / 2;
            colbox.width = spread;
            colbox.height = spread;

            for (Players::iterator it = players.begin(); it != players.end(); it++) {
                Player *p = *it;
                CollisionBox p_colbox = p->get_characterset()->get_colbox();
                p_colbox.x += static_cast<int>(p->state.client_server_state.x);
                p_colbox.y = static_cast<int>(p->state.client_server_state.y) - p_colbox.height - p_colbox.y;

                if (p->is_alive_and_playing()) {
                    if (colbox.intersects(p_colbox))
                    {
                        player_damage(owner, p, npc, ani->get_damage(), ani->get_name());
                    }
                }
            }

            for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
                it != spawnable_npcs.end(); it++)
            {
                SpawnableNPC *npc = *it;
                CollisionBox n_colbox = npc->npc->get_damage_colbox();
                n_colbox.x += static_cast<int>(npc->state.x);
                n_colbox.y = static_cast<int>(npc->state.y) - n_colbox.height - n_colbox.y;
                if (colbox.intersects(n_colbox)) {
                    npc->delete_me = true;
                    add_npc_remove_animation(npc);
                    send_remove_npc(npc);
                }
            }
            if (!preserve_npc) {
                remove_marked_npcs();
            }
        }
    }
}
