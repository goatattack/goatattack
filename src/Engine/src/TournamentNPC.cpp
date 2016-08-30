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
#include <algorithm>

void Tournament::reset_frog_spawn_counter() {
    frog_respawn_counter = static_cast<double>(rand() % frog_spawn_init + frog_spawn_init);
}

void Tournament::add_spawnable_npc(GSpawnNPC *snpc) {
    NPC *npc = resources.get_npc(snpc->npc_name);

    SpawnableNPC *nnpc = new SpawnableNPC;
    nnpc->npc = npc;
    nnpc->state.id = snpc->id;
    nnpc->state.owner = snpc->owner;
    nnpc->state.flags = 0;
    nnpc->state.direction = snpc->direction;
    nnpc->state.x = snpc->x;
    nnpc->state.y = snpc->y;
    nnpc->state.accel_x = snpc->accel_x;
    nnpc->state.accel_y = snpc->accel_y;
    nnpc->icon = NPCAnimationStanding;
    nnpc->iconindex = 0;
    nnpc->init_owner = snpc->owner;
    nnpc->ignore_owner_counter = npc->get_ignore_owner_counter();
    nnpc->move_counter = static_cast<double>(npc->get_move_init_randomized());
    spawnable_npcs.push_back(nnpc);

    if (!server) {
        try {
            if (snpc->sound_name[0]) {
                subsystem.play_sound(resources.get_sound(snpc->sound_name), 0);
            }
        } catch (const Exception& e) {
            subsystem << e.what();
        }
    }
}

void Tournament::remove_spawnable_npc(GRemoveNPC *rnpc) {
    for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
        it != spawnable_npcs.end(); it++)
    {
        SpawnableNPC *npc = *it;
        if (npc->state.id == rnpc->id) {
            spawnable_npcs.erase(it);
            delete npc;
            break;
        }
    }
}

void Tournament::remove_marked_npcs() {
    spawnable_npcs.erase(std::remove_if(spawnable_npcs.begin(),
        spawnable_npcs.end(), erase_element<SpawnableNPC>),
        spawnable_npcs.end());
}

void Tournament::send_remove_npc(SpawnableNPC *npc) {
    GRemoveNPC *rnpc = new GRemoveNPC;
    rnpc->id = npc->state.id;
    rnpc->to_net();
    add_state_response(GPCRemoveNPC, GRemoveNPCLen, rnpc);
}


void Tournament::add_npc_remove_animation(SpawnableNPC *npc) {
    const std::string& animation_name = npc->npc->get_value("kill_animation");
    const std::string& sound_name = npc->npc->get_value("kill_sound");
    if (animation_name.length()) {
        try {
            Animation *ani = resources.get_animation(animation_name);
            TileGraphic *tg = npc->npc->get_tile(DirectionLeft, NPCAnimationStanding)->get_tilegraphic();
            int width = tg->get_width();
            int height = tg->get_height();
            GAnimation *sgani = new GAnimation;
            memset(sgani, 0, GAnimationLen);
            strncpy(sgani->animation_name, ani->get_name().c_str(), NameLength - 1);
            strncpy(sgani->sound_name, sound_name.c_str(), NameLength - 1);
            sgani->id = npc->state.id;
            sgani->duration = ani->get_duration();
            TileGraphic *anitg = ani->get_tile()->get_tilegraphic();

            sgani->x = npc->state.x + width / 2 - anitg->get_width() / 2;
            sgani->y = npc->state.y - height / 2 - anitg->get_height() / 2;
            sgani->accel_x = 0.0f;
            sgani->accel_y = 0.0f;
            sgani->to_net();
            add_state_response(GPCAddAnimation, GAnimationLen, sgani);
        } catch (const Exception& e) {
            subsystem << e.what() << std::endl;
        }
    }
}

void Tournament::spawn_frog() {
    size_t sz = frog_spawn_points.size();
    if (sz) {
        /* if a frog is already in map, leave */
        for (GameObjects::iterator it = game_objects.begin();
            it != game_objects.end(); it++)
        {
            GameObject *gobj = *it;
            if (gobj->object->get_type() == Object::ObjectTypeFrog) {
                return;
            }
        }

        /* spawn object */
        try {
            Object *obj = resources.get_object("frog");
            int index = rand() % sz;
            GameObject *gobj = frog_spawn_points[index];

            identifier_t id = get_free_object_id();
            spawn_object(obj, id, gobj->origin_x, gobj->origin_y, 0);

            GSpawnObject *spwn = new GSpawnObject;
            memset(spwn, 0, sizeof(GSpawnObject));
            spwn->flags = PlaceObjectWithAnimation | PlaceObjectWithSpawnSound;
            spwn->id = id;
            strncpy(spwn->object_name, obj->get_name().c_str(), NameLength - 1);
            spwn->x = static_cast<pos_t>(gobj->origin_x);
            spwn->y = static_cast<pos_t>(gobj->origin_y);
            spwn->to_net();
            add_state_response(GPCSpawnObject, GSpawnObjectLen, spwn);
        } catch (const Exception& e) {
            subsystem << e.what() << std::endl;
        }
    }
}

identifier_t Tournament::get_free_object_id() {
    identifier_t id = 0;
    bool found;
    do {
        found = false;
        size_t sz = game_objects.size();
        for (size_t i = 0; i < sz; i++) {
            GameObject *obj = game_objects[i];
            if (obj->state.id == id) {
                found = true;
                break;
            }
        }
        if(found) {
            id++;
        }
    } while (found);

    return id;
}

identifier_t Tournament::get_free_npc_id() {
    identifier_t id = 0;
    bool found;
    do {
        found = false;
        size_t sz = spawnable_npcs.size();
        for (size_t i = 0; i < sz; i++) {
            SpawnableNPC *npc = spawnable_npcs[i];
            if (npc->state.id == id) {
                found = true;
                break;
            }
        }
        if(found) {
            id++;
        }
    } while (found);

    return id;
}

void Tournament::update_npc_states(double period_f) {
    for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
        it != spawnable_npcs.end(); it++)
    {
        SpawnableNPC *npc = *it;

        /* update physics */
        TileGraphic *tg = npc->npc->get_tile(DirectionLeft, NPCAnimationStanding)->get_tilegraphic();
        int width = tg->get_width();
        int height = tg->get_height();

        const CollisionBox& colbox = npc->npc->get_colbox();

        bool killing = false;
        double y = npc->state.y - height;
        bool col = render_physics(period_f, false, 0, 0.0f, 0,
            npc->npc->get_springiness_x(), 0.0f, colbox,
            npc->state.x, y, npc->state.accel_x, npc->state.accel_y,
            width, height, npc->npc->get_friction_factor(), false, npc->falling,
            npc->last_falling_y_pos, &killing, npc->npc->get_name());
        npc->state.y = y + height;

        /* update ownership */
        if (server && npc->init_owner) {
            npc->ignore_owner_counter -= period_f;
            if (npc->ignore_owner_counter <= 0.0f) {
                npc->init_owner = 0;
            }
        }

        /* update motion */
        Sound *idle_sound = 0;
        if (!npc->in_idle) {
            npc->move_counter -= period_f;
            if (npc->move_counter <= 0.0f) {
                npc->move_counter = static_cast<double>(npc->npc->get_move_init_randomized());

                int action = rand() % 5;

                switch (action) {
                    case 0:
                    case 1:
                    {
                        /* jump */
                        if (server) {
                            if (!npc->falling) {
                                npc->state.accel_y = -npc->npc->get_jump_y_impulse_randomized();
                                npc->state.accel_x = npc->npc->get_jump_x_impulse_randomized() *
                                    (static_cast<Direction>(npc->state.direction) == DirectionLeft ? -1 : 1);
                            }
                        }
                        break;
                    }

                    case 2:
                    {
                        /* change direction */
                        if (server && !col && !npc->falling) {
                            npc->state.direction = static_cast<unsigned char>(rand() % 2 ? DirectionLeft : DirectionRight);
                        }
                        break;
                    }

                    case 3:
                    {
                        /* idle 1 */
                        if (!server) {
                            npc->in_idle = true;
                            npc->idle_counter_init = npc->npc->get_idle1_counter();
                            npc->idle_counter = npc->idle_counter_init;
                            npc->icon = NPCAnimationIdle1;
                            npc->iconindex = 0;
                            const std::string& sound = npc->npc->get_value("idle1_sound");
                            if (sound.length()) {
                                try {
                                    idle_sound = resources.get_sound(sound);
                                } catch (const Exception& e) {
                                    subsystem << e.what() << std::endl;
                                }
                            }
                        }
                        break;
                    }

                    case 4:
                    {
                        /* idle 2 */
                        if (!server) {
                            npc->in_idle = true;
                            npc->idle_counter_init = npc->npc->get_idle2_counter();
                            npc->idle_counter = npc->idle_counter_init;
                            npc->icon = NPCAnimationIdle2;
                            npc->iconindex = 0;
                            const std::string& sound = npc->npc->get_value("idle2_sound");
                            if (sound.length()) {
                                try {
                                    idle_sound = resources.get_sound(sound);
                                } catch (const Exception& e) {
                                    subsystem << e.what() << std::endl;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        /* max x accel? */
        double max_accel_x = npc->npc->get_max_accel_x();
        if (npc->state.accel_x > max_accel_x) {
            npc->state.accel_x = max_accel_x;
        }
        if (npc->state.accel_x < -max_accel_x) {
            npc->state.accel_x = -max_accel_x;
        }

        if (server) {
            if (npc->state.y - 100 > map_height * tile_height) {
                npc->delete_me = true;
                send_remove_npc(npc);
            } else if (killing) {
                npc->delete_me = true;
                add_npc_remove_animation(npc);
                send_remove_npc(npc);
            }
        }

        /* update icon */
        if (npc->falling) {
            npc->in_idle = false;
            npc->icon = NPCAnimationJumping;
            npc->iconindex = 0;
            if (col && server) {
                if (npc->state.direction == static_cast<unsigned char>(DirectionLeft)) {
                    npc->state.direction = static_cast<unsigned char>(DirectionRight);
                } else {
                    npc->state.direction = static_cast<unsigned char>(DirectionLeft);
                }
            }
        } else {
            if (!npc->in_idle) {
                npc->icon = NPCAnimationStanding;
                npc->iconindex = 0;
            }
        }

        /* update idle */
        if (npc->in_idle) {
            if (idle_sound) {
                subsystem.play_sound(idle_sound, 0);
            }
            npc->idle_counter -= period_f;
            if (npc->idle_counter <= 0.0f) {
                npc->iconindex++;
                npc->idle_counter = npc->idle_counter_init;
                Tile *t =npc->npc->get_tile(static_cast<Direction>(npc->state.direction),
                    npc->icon);
                TileGraphic *tg = t->get_tilegraphic();
                if (static_cast<size_t>(npc->iconindex) >= tg->get_tile_count()) {
                    npc->in_idle = false;
                    npc->icon = NPCAnimationStanding;
                    npc->iconindex = 0;
                }
            }
        }
    }

    remove_marked_npcs();
}

void Tournament::player_npc_collision(Player *p, SpawnableNPC *npc) {
    if (npc->init_owner != p->state.id) {
        const std::string& explosion_animation = npc->npc->get_value("explosion_animation");
        if (explosion_animation.length()) {
            Animation *ani = resources.get_animation(explosion_animation);
            GAnimation *sgani = new GAnimation;
            memset(sgani, 0, GAnimationLen);
            strncpy(sgani->animation_name, ani->get_name().c_str(), NameLength - 1);
            strncpy(sgani->sound_name, ani->get_value("sound_name").c_str(), NameLength - 1);
            sgani->id = npc->state.id;
            sgani->duration = ani->get_duration();
            sgani->x = npc->state.x + ani->get_x_offset();
            sgani->y = npc->state.y + ani->get_y_offset();
            sgani->accel_x = 0.0f;
            sgani->accel_y = 0.0f;
            sgani->to_net();
            add_state_response(GPCAddAnimation, GAnimationLen, sgani);
            check_killing_animation(static_cast<int>(npc->state.x),
                static_cast<int>(npc->state.y), ani, npc->state.owner, true, npc->npc);
        }
    }
}
