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

#include <cmath>
#include <algorithm>

static double bump_sound_velocity = 0.35f;

bool Tournament::render_physics(double period_f, bool projectile, int damage,
    double recoil, identifier_t owner, double springiness_x, double springiness_y,
    const CollisionBox& colbox, double& x, double& y, double& accel_x,
    double& accel_y, int width, int height, double friction_factor,
    bool play_bump, bool& falling, int& last_falling_y_pos,
    bool *killing, const std::string& weapon)
{
    bool is_collision = false;

    /* gravity */
    if (!projectile) {
        accel_y += YAccelGravity * period_f;
        if (accel_y > YMaxAccel) {
            accel_y = YMaxAccel;
        }
    }

    /* new position */
    double newx = x + (accel_x * period_f);
    double newy = y + (accel_y * period_f);

    /* map boundary checks */
    if (newx < -colbox.x) {
        newx = -colbox.x;
        accel_x = -accel_x * springiness_x;
        is_collision = true;
    }
    if (newx + colbox.x + colbox.width > map_width * tile_width) {
        newx = map_width * tile_width - (colbox.x + colbox.width);
        accel_x = -accel_x * springiness_x;
        is_collision = true;
    }

    /* tile collision detection */
    bool bailout;
    int colmax;

    /* AABB tile collision detection in x direction */
    do {
        /* left side */
        if (accel_x < 0.0f) {
            /* left upper stepped down */
            bailout = false;
            colmax = ((colbox.height - 1) / tile_height) + 1;
            for (int i = 0; i < colmax; i++) {
                if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                    newx + colbox.x, y + height - colbox.y - colbox.height +
                    (i * tile_height), 0, killing))
                {
                    newx = (tilex + 1) * tile_width - colbox.x;
                    if (accel_x < -bump_sound_velocity && play_bump) {
                        play_ground_bump_sound();
                    }
                    accel_x = -accel_x * springiness_x;
                    is_collision = true;
                    bailout = true;
                    break;
                }
            }
            if (bailout) {
                break;
            }

            /* left lower */
            if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                newx + colbox.x, y + height - colbox.y - 1.0f, 0, killing))
            {
                newx = (tilex + 1) * tile_width - colbox.x;
                if (accel_x < -bump_sound_velocity && play_bump) {
                    play_ground_bump_sound();
                }
                accel_x = -accel_x * springiness_x;
                is_collision = true;
                break;
            }
        }

        /* right side */
        if (accel_x > 0.0f) {
            /* right upper stepped down */
            bailout = false;
            colmax = ((colbox.height - 1) / tile_height) + 1;
            for (int i = 0; i < colmax; i++) {
                if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                    newx + colbox.width + colbox.x, y + height - colbox.height -
                    colbox.y + (i * tile_height), 0, killing))
                {
                    newx = tilex * tile_width - colbox.width - colbox.x;
                    if (accel_x > bump_sound_velocity && play_bump) {
                        play_ground_bump_sound();
                    }
                    accel_x = -accel_x * springiness_x;
                    is_collision = true;
                    bailout = true;
                    break;
                }
            }
            if (bailout) {
                break;
            }

            /* right lower */
            if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                newx + colbox.width + colbox.x, y + width - colbox.y - 1.0f, 0, killing))
            {
                newx = tilex * tile_width - colbox.width - colbox.x;
                if (accel_x > bump_sound_velocity && play_bump) {
                    play_ground_bump_sound();
                }
                accel_x = -accel_x * springiness_x;
                is_collision = true;
            }
        }
    } while (false);

    /* save last falling y before y correction for falling tile collision tests */
    if (static_cast<int>(newy + height) < last_falling_y_pos) {
        last_falling_y_pos = static_cast<int>(newy + height);
    }

    /* AABB tile collision detection in y direction */
    double ground_friction = 0.0f;
    bool new_falling = true;
    do {
        /* test if ascending only */
        if (accel_y < 0.0f) {
            last_falling_y_pos = Player::PlayerFallingTestMaxY;
            /* top edge */
            bailout = false;
            colmax = ((colbox.width - 1) / tile_width) + 1;
            for (int i = 0; i < colmax; i++) {
                if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                    x + colbox.x + (i * tile_width), newy + height - colbox.y -
                    colbox.height, 0, killing))
                {
                    newy = (tiley + 1) * tile_height + colbox.y + colbox.height - height;
                    if (accel_y < -bump_sound_velocity && play_bump) {
                        play_ground_bump_sound();
                    }
                    accel_y = -accel_y * springiness_y;
                    is_collision = true;
                    new_falling = false;
                    bailout = true;
                    break;
                }
            }
            if (bailout) {
                break;
            }

            /* top edge right point */
            if (collide_with_tile(TestTypeNormal, 0, last_falling_y_pos,
                x + colbox.x + colbox.width - 1.0f, newy + height - colbox.y -
                colbox.height, 0, killing))
            {
                newy = (tiley + 1) * tile_height + colbox.y + colbox.height - height;
                if (accel_y < -bump_sound_velocity && play_bump) {
                    play_ground_bump_sound();
                }
                accel_y = -accel_y * springiness_y;
                is_collision = true;
                new_falling = false;
                break;
            }
        }

        /* test if falling only */
        if (accel_y > 0.0f) {
            /* bottom edge */
            bool found = false;
            colmax = ((colbox.width - 1) / tile_width) + 1;
            for (int i = 0; i < colmax; i++) {
                double tile_friction = 0.0f;
                if (collide_with_tile(TestTypeFalling, 0, last_falling_y_pos,
                    x + colbox.x + (i * tile_width), newy + height - colbox.y,
                    &tile_friction, killing))
                {
                    if (!found) {
                        newy = tiley * tile_height + colbox.y - height;
                        if (accel_y > bump_sound_velocity && play_bump) {
                            play_ground_bump_sound();
                        }
                        accel_y = -accel_y * springiness_y;
                        is_collision = true;
                        new_falling = false;
                        found = true;
                    }
                }
                if (tile_friction > ground_friction) {
                    ground_friction = tile_friction;
                }
            }

            /* bottom edge right point */
            double tile_friction = 0.0f;
            if (collide_with_tile(TestTypeFalling, 0, last_falling_y_pos,
                x + colbox.x + colbox.width - 1.0f, newy + height - colbox.y,
                &tile_friction, killing))
            {
                if (!found) {
                    newy = tiley * tile_height + colbox.y - height;
                    if (accel_y > bump_sound_velocity && play_bump) {
                        play_ground_bump_sound();
                    }
                    accel_y = -accel_y * springiness_y;
                    is_collision = true;
                    new_falling = false;
                }
            }
            if (tile_friction > ground_friction) {
                ground_friction = tile_friction;
            }
        }
    } while (false);

    /* save last falling y after y correction for falling tile collision tests */
    if (static_cast<int>(newy + height) > last_falling_y_pos) {
        last_falling_y_pos = static_cast<int>(newy + height);
    }

    /* save flag */
    falling = new_falling;

    /* object is sliding -> more friction */
    ground_friction *= friction_factor;
    if (!falling) {
        if (accel_x > -Epsilon && accel_x < Epsilon) {
            accel_x = 0.0f;
        } else if (accel_x < -Epsilon) {
            accel_x += (ground_friction * period_f);
            if (accel_x > -Epsilon) {
                accel_x = 0.0f;
            }
        } else if (accel_x > Epsilon) {
            accel_x -= (ground_friction * period_f);
            if (accel_x < Epsilon) {
                accel_x = 0.0f;
            }
        }
    }

    /* new position */
    x = newx;
    y = newy;

    /* projectile -> player or npc hit? */
    if (projectile) {
        CollisionBox obj_colbox = colbox;
        obj_colbox.x += static_cast<int>(x);
        obj_colbox.y = static_cast<int>(y) + height - obj_colbox.height - obj_colbox.y;

        /* test players */
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if (p->state.id != owner) {
                if (p->is_alive_and_playing()) {
                    CollisionBox p_colbox = p->get_characterset()->get_damage_colbox();
                    p_colbox.x += static_cast<int>(p->state.client_server_state.x);
                    p_colbox.y = static_cast<int>(p->state.client_server_state.y) - p_colbox.height - p_colbox.y;
                    if (obj_colbox.intersects(p_colbox)) {
                        if (server) {
                            GPlayerRecoil *prec = new GPlayerRecoil;
                            prec->id = p->state.id;
                            prec->x_recoil = (accel_x < 0.0f ? recoil : -recoil);
                            prec->to_net();
                            add_state_response(GPCPlayerRecoil, GPlayerRecoilLen, prec);
                            player_damage(owner, p, 0, damage, weapon);
                        }
                        is_collision = true;
                    }
                }
            }
        }

        /* test npcs */
        for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
            it != spawnable_npcs.end(); it++)
        {
            SpawnableNPC *npc = *it;
            CollisionBox n_colbox = npc->npc->get_damage_colbox();
            n_colbox.x += static_cast<int>(npc->state.x);
            n_colbox.y = static_cast<int>(npc->state.y) - n_colbox.height - n_colbox.y;
            if (obj_colbox.intersects(n_colbox)) {
                if (server) {
                    add_npc_remove_animation(npc);
                    npc->delete_me = true;
                    send_remove_npc(npc);
                }
                is_collision = true;
            }
        }

        remove_marked_npcs();
    }

    return is_collision;
}
