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

namespace {

    const double YInitialJumpImpulse = 3.8;
    const double YVeloJumpToLanding = 2.0;
    const ns_t IntegrateMaxTimeStep = 10000000;

}

extern const int MaxBombs;
extern const int MaxFrogs;
extern const int MaxArmor;
extern const int MaxGrenades;
extern const int MaxHealth;
extern const int MaxAmmo;

void Tournament::update_states(ns_t ns) {
    if (!ready) {
        return;
    }

    ns_t shot = IntegrateMaxTimeStep;
    while (ns) {
        if (ns <= IntegrateMaxTimeStep) {
            shot = ns;
            ns = 0;
        } else {
            ns -= IntegrateMaxTimeStep;
        }
        integrate(shot);
    }
}

void Tournament::integrate(ns_t ns) {
    double period_f = ns / static_cast<double>(ns_fc);

    /* in the lobby? */
    if (game_state.flags & GameStateFlagLobby) {
        Player *me = get_me();
        if (!server && me) {
            if (!(me->state.server_state.flags & PlayerServerFlagIsReady)) {
                // ready in lobby?
                bool button_state = ((me->state.client_server_state.key_states & PlayerKeyStateFire) != 0);
                if (button_state != last_button_b_state) {
                    last_button_b_state = button_state;
                    if (button_state) {
                        add_state_response(GPSLobbyReadyRequest, 0, 0);
                    }
                }
                if (is_team_tournament()) {
                    button_state = ((me->state.client_server_state.key_states & PlayerKeyStateJump) != 0);
                    if (button_state != last_button_team_select_state) {
                        last_button_team_select_state = button_state;
                        if (button_state) {
                            if (me->state.server_state.flags & PlayerServerFlagTeamRed) {
                                add_state_response(GPSLobbyTeamBlueSelect, 0, 0);
                            } else {
                                add_state_response(GPSLobbyTeamRedSelect, 0, 0);
                            }
                        }
                    }
                }
            }
        }
        return;
    }

    /* show stats, if game is over? */
    if (!game_state.seconds_remaining) {
        if (!warmup) {
            game_over = true;
            show_statistics = true;
        }
        return;
    }

    /* update time counter */
    if (server) {
        second_counter += ns;
        if (second_counter >= ns_sec) {
            second_counter -= ns_sec;
            if (game_state.seconds_remaining) {
                game_state.seconds_remaining--;
                if (!game_state.seconds_remaining) {
                    if (!warmup) {
                        add_state_response(GPCGameOver, 0, 0);
                        if (logger) {
                            logger->log(ServerLogger::LogTypeGameOver, i18n(I18N_TNMT_STATS_OVER));
                            write_stats_in_server_log();
                            logger->log(ServerLogger::LogTypeEndOfStats, i18n(I18N_TNMT_STATS_END));
                        }
                    }
                }
            }
        }
    }

    /* update game objects -> respawning */
    if (server) {
        for (GameObjects::iterator it = game_objects.begin();
            it != game_objects.end(); it++)
        {
            GameObject *obj = *it;
            int spawn_time = obj->object->get_spawning_time();
            if (obj->picked && spawn_time) {
                obj->spawn_counter += period_f;
                if (obj->spawn_counter >= spawn_time) {
                    obj->picked = false;
                    obj->spawn_counter = 0.0f;
                    GPlaceObject *gpo = new GPlaceObject;
                    gpo->id = obj->state.id;
                    gpo->flags = PlaceObjectWithAnimation | PlaceObjectWithSpawnSound;
                    gpo->x = static_cast<pos_t>(obj->state.x);
                    gpo->y = static_cast<pos_t>(obj->state.y);
                    gpo->to_net();
                    add_state_response(GPCPlaceObject, GPlaceObjectLen, gpo);
                }
            }
        }
    }

    /* update typing animation */
    player_afk_counter += period_f * AnimationMultiplier;
    if (player_afk_counter > player_afk->get_animation_speed()) {
        player_afk_counter = 0.0f;
        player_afk_index = player_afk_index + 1;
        if (player_afk_index >= player_afk->get_tile()->get_tilegraphic()->get_tile_count()) {
            player_afk_index = 0;
        }
    }

    /* update animation states and its physics */
    for (GameAnimations::iterator it = game_animations.begin();
        it != game_animations.end(); it++)
    {
        GameAnimation *gani = *it;

        gani->animation_counter += period_f * AnimationMultiplier;
        double speed = static_cast<double>(gani->animation->get_animation_speed());
        bool finished = false;

        if (gani->animation_counter > speed) {
            TileGraphic *tg = gani->animation->get_tile()->get_tilegraphic();
            gani->animation_counter = 0.0f;
            gani->index++;
            if (gani->index >= static_cast<int>(tg->get_tile_count())) {
                if (gani->state.duration) {
                    gani->state.duration--;
                    if (!gani->state.duration) {
                        finished = true;
                    }
                } else {
                    finished = true;
                }
            }
        }

        if (gani->animation->get_physics()) {
            TileGraphic *tg = gani->animation->get_tile()->get_tilegraphic();
            int width = tg->get_width();
            int height = tg->get_height();
            double springiness = gani->animation->get_springiness();
            bool projectile = gani->animation->is_projectile();
            double recoil = gani->animation->get_recoil();
            int damage = gani->animation->get_damage();

            const CollisionBox& colbox = gani->animation->get_physics_colbox();
            bool col = render_physics(period_f, projectile, damage, recoil,
                gani->state.owner, springiness, springiness, colbox,
                gani->state.x, gani->state.y, gani->state.accel_x,
                gani->state.accel_y, width, height, 1.0f, true, gani->falling,
                gani->last_falling_y_pos, 0, gani->animation->get_name());

            if (col && projectile) {
                finished = true;
            }
        }

        if (finished) {
            gani->delete_me = true;
            if (server) {
                identifier_t *id = new identifier_t;
                *id = htons(gani->state.id);
                add_state_response(GPCRemoveAnimation, sizeof(id), id);
                const std::string& finished_animation = gani->animation->get_value("finished_animation");
                if (finished_animation.length()) {
                    Animation *ani = resources.get_animation(finished_animation);
                    GAnimation *sgani = new GAnimation;
                    memset(sgani, 0, GAnimationLen);
                    strncpy(sgani->animation_name, ani->get_name().c_str(), NameLength - 1);
                    strncpy(sgani->sound_name, ani->get_value("sound_name").c_str(), NameLength - 1);
                    sgani->id = gani->state.id;
                    sgani->duration = ani->get_duration();
                    sgani->x = gani->state.x + ani->get_x_offset();
                    sgani->y = gani->state.y + ani->get_y_offset();
                    sgani->accel_x = 0.0f;
                    sgani->accel_y = 0.0f;
                    sgani->to_net();
                    add_state_response(GPCAddAnimation, GAnimationLen, sgani);
                    check_killing_animation(
                        static_cast<int>(gani->state.x + gani->animation->get_tile()->get_tilegraphic()->get_width() / 2),
                        static_cast<int>(gani->state.y + gani->animation->get_tile()->get_tilegraphic()->get_height() / 2),
                        ani, gani->state.owner,
                        false, 0
                    );
                }
            }
        }
    }

    game_animations.erase(std::remove_if(game_animations.begin(),
        game_animations.end(), erase_element<GameAnimation>),
        game_animations.end());

    /* update text animations */
    for (GameTextAnimations::iterator it = game_text_animations.begin();
        it != game_text_animations.end(); it++)
    {
        GameTextAnimation *gani = *it;
        gani->animation_counter += period_f * AnimationMultiplier;
        if (gani->animation_counter > TextAnimationSpeed) {
            gani->animation_counter = 0.0f;
            gani->y--;
            gani->rise_counter++;
            if (gani->rise_counter > gani->max_rise_counter) {
                gani->delete_me = true;
            }
        }
    }

    game_text_animations.erase(std::remove_if(game_text_animations.begin(),
        game_text_animations.end(), erase_element<GameTextAnimation>),
        game_text_animations.end());

    /* update frog respawns */
    if (server && has_frogs) {
        frog_respawn_counter -= period_f;
        if (frog_respawn_counter <= 0.0f) {
            reset_frog_spawn_counter();
            spawn_frog();
        }
    }

    /* update npcs */
    update_npc_states(period_f);

    /* update tile states */
    if (!server) {
        resources.update_tile_index(period_f * AnimationMultiplier, tileset);
    }

    /* update object physics */
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        if (!obj->picked) {
            if (obj->object->get_physics()) {
                TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();
                int width = tg->get_width();
                int height = tg->get_height();
                double springiness = obj->object->get_springiness();

                const CollisionBox& colbox = (obj->object->has_physics_colbox() ?
                    obj->object->get_physics_colbox() : obj->object->get_colbox());

                render_physics(period_f, false, 0, 0.0f, 0, springiness,
                    springiness, colbox, obj->state.x, obj->state.y, obj->state.accel_x,
                    obj->state.accel_y, width, height, 1.0f, true, obj->falling,
                    obj->last_falling_y_pos, 0, obj->object->get_name());
            }
        }
    }

    /* subclassed integration */
    subintegrate(ns);

    /* player update cycle */
    Player *following_player = 0;
    Player *me = get_me();

    /* player wants to join or to respawn */
    if (!server && me) {
        bool button_state = ((me->state.client_server_state.key_states & PlayerKeyStateJump) != 0);
        if (button_state != last_button_a_state) {
            last_button_a_state = button_state;
            if (me->state.server_state.flags & PlayerServerFlagSpectating) {
                if (!me->joining && button_state) {
                    player_join_request(me);
                }
            } else if (!me->is_alive_and_playing()) {
                if (!me->respawning && button_state) {
                    this->spawn_player(me);
                    me->respawning = true;
                    add_state_response(GPSRespawnRequest, 0, 0);
                    me->state.client_state.flags &= ~PlayerClientFlagJumpReleased;
                }
            }
        }
    }

    /* control spectator */
    control_spectator(me, period_f);

    /* update all player states */
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;

        if (warmup && server) {
            p->state.server_state.flags |= PlayerServerFlagHasShotgunBelt;
            p->state.server_state.ammo = MaxAmmo;
            p->state.server_state.armor = MaxArmor;
            p->state.server_state.bombs = MaxBombs;
            p->state.server_state.frogs = MaxFrogs;
            p->state.server_state.grenades = MaxGrenades;
        }

        if (p->is_alive_and_playing()) {
            const CollisionBox& colbox = Characterset::Colbox;

            /* check player name width */
            if (!p->font) {
                p->font = resources.get_font("normal");
                p->player_name_width = p->font->get_text_width(p->get_player_name());
            }

            /* set following player */
            if (p->state.id == following_id) {
                following_player = p;
            }

            /* action key triggers */
            bool move_left = ((p->state.client_server_state.key_states & PlayerKeyStateLeft) != 0) |
                ((p->state.client_server_state.jaxis & PlayerKeyStateLeft) != 0);

            bool move_right = ((p->state.client_server_state.key_states & PlayerKeyStateRight) != 0) |
                ((p->state.client_server_state.jaxis & PlayerKeyStateRight) != 0);

            bool move_up = ((p->state.client_server_state.key_states & PlayerKeyStateUp) != 0) |
                ((p->state.client_server_state.jaxis & PlayerKeyStateUp) != 0);

            bool move_down = ((p->state.client_server_state.key_states & PlayerKeyStateDown) != 0) |
                ((p->state.client_server_state.jaxis & PlayerKeyStateDown) != 0);

            bool move_jump = ((p->state.client_server_state.key_states & PlayerKeyStateJump) != 0);

            /* prevent locked slidings */
            if (move_left && move_right) {
                move_left = move_right = false;
            }

            if (move_up && move_down) {
                move_up = move_down = false;
            }

            /* horizontal acceleration */
            if (move_left) {
                p->state.client_server_state.accel_x -= (XAccel * period_f);
            }

            if (move_right) {
                p->state.client_server_state.accel_x += (XAccel * period_f);
            }

            if (p->state.client_server_state.accel_x < -XMaxAccel) {
                p->state.client_server_state.accel_x = -XMaxAccel;
            }

            if (p->state.client_server_state.accel_x > XMaxAccel) {
                p->state.client_server_state.accel_x = XMaxAccel;
            }

            /* horizontal deceleration */
            if (!move_left && !move_right) {
                if (p->state.client_server_state.accel_x > -Epsilon && p->state.client_server_state.accel_x < Epsilon) {
                    p->state.client_server_state.accel_x = 0.0f;
                } else if (p->state.client_server_state.accel_x < -Epsilon) {
                    p->state.client_server_state.accel_x += (XDecel * period_f);
                    if (p->state.client_server_state.accel_x > -Epsilon) {
                        p->state.client_server_state.accel_x = 0.0f;
                    }
                } else if (p->state.client_server_state.accel_x > Epsilon) {
                    p->state.client_server_state.accel_x -= (XDecel * period_f);
                    if (p->state.client_server_state.accel_x < Epsilon) {
                        p->state.client_server_state.accel_x = 0.0f;
                    }
                }
            }

            /* jump */
            if (move_jump && p->state.client_state.flags & PlayerClientFlagJumpReleased) {
                p->state.client_state.flags &= ~PlayerClientFlagJumpReleased;
                if (!(p->state.client_state.flags & PlayerClientFlagFalling) &&
                    p->state.client_server_state.jump_accel_y > -Epsilon &&
                    p->state.client_server_state.jump_accel_y < Epsilon)
                {
                    if (p == me) {
                        p->state.client_server_state.jump_accel_y = -YInitialJumpImpulse;
                        if (!server) {
                            p->force_broadcast = true;
                            subsystem.play_sound(resources.get_sound(Characterset::JumpSound), 0);
                        }
                    }
                }
            } else if (!move_jump) {
                p->state.client_state.flags |= PlayerClientFlagJumpReleased;
            }

            /* gravity */
            p->state.client_server_state.accel_y += YAccelGravity * period_f;
            if (p->state.client_server_state.accel_y > YMaxAccel) {
                p->state.client_server_state.accel_y = YMaxAccel;
            }

            if (p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y < -Epsilon) {
                p->state.client_server_state.jump_accel_y += (move_jump ? YDecelJump : YDecelJumpNormal) * period_f;
            } else {
                p->state.client_server_state.jump_accel_y += YDecelJumpNormal * period_f;
            }

            if (p->state.client_server_state.jump_accel_y > -Epsilon) {
                p->state.client_server_state.jump_accel_y = 0.0f;
            }

            /* new movement vector */
            double movy = p->state.client_server_state.accel_y + p->state.client_server_state.jump_accel_y;

            /* new position */
            double newx = p->state.client_server_state.x + (p->state.client_server_state.accel_x * period_f);
            double newy = p->state.client_server_state.y + (movy * period_f);

            /* map boundary checks */
            if (newx < -colbox.x) {
                newx = -colbox.x;
                p->state.client_server_state.accel_x = 0.0f;
            }
            if (newx + colbox.x + colbox.width > map_width * tile_width) {
                newx = map_width * tile_width - (colbox.x + colbox.width);
                p->state.client_server_state.accel_x = 0.0f;
            }

            /* tile collision detection */
            bool bailout;
            int colmax;

            /* AABB tile collision detection in x direction */
            do {
                /* left side */
                if (p->state.client_server_state.accel_x < 0.0f) {
                    /* left upper stepped down */
                    bailout = false;
                    colmax = ((colbox.height - 1) / tile_height) + 1;
                    for (int i = 0; i < colmax; i++) {
                        if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, newx + colbox.x, p->state.client_server_state.y - colbox.y - colbox.height + (i * tile_height), 0, 0)) {
                            newx = (tilex + 1) * tile_width - colbox.x;
                            p->state.client_server_state.accel_x = 0.0f;
                            bailout = true;
                            break;
                        }
                    }
                    if (bailout) {
                        break;
                    }

                    /* left lower */
                    if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, newx + colbox.x, p->state.client_server_state.y - colbox.y - 1.0f, 0, 0)) {
                        newx = (tilex + 1) * tile_width - colbox.x;
                        p->state.client_server_state.accel_x = 0.0f;
                        break;
                    }
                }

                if (p->state.client_server_state.accel_x > 0.0f) {
                    /* right upper stepped down */
                    bailout = false;
                    colmax = ((colbox.height - 1) / tile_height) + 1;
                    for (int i = 0; i < colmax; i++) {
                        if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, newx + colbox.width + colbox.x, p->state.client_server_state.y - colbox.height - colbox.y + (i * tile_height), 0, 0)) {
                            newx = tilex * tile_width - colbox.width - colbox.x;
                            p->state.client_server_state.accel_x = 0.0f;
                            bailout = true;
                            break;
                        }
                    }
                    if (bailout) {
                        break;
                    }

                    /* right lower */
                    if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, newx + colbox.width + colbox.x, p->state.client_server_state.y - colbox.y - 1.0f, 0, 0)) {
                        newx = tilex * tile_width - colbox.width - colbox.x;
                        p->state.client_server_state.accel_x = 0.0f;
                    }
                }
            } while (false);

            /* set player's falling flag */
            p->state.client_state.flags |= PlayerClientFlagFalling;

            /* save last falling y before y correction for falling tile collision tests */
            if (static_cast<int>(newy) < p->last_falling_y_pos) {
                p->last_falling_y_pos = static_cast<int>(newy);
            }

            /* AABB tile collision detection in y direction */
            double ground_friction = 0.0f;

            do {
                /* test if jumping only */
                if (movy < 0.0f) {
                    /* reset after jump */
                    p->last_falling_y_pos = Player::PlayerFallingTestMaxY;
                    /* top edge */
                    bailout = false;
                    colmax = ((colbox.width - 1) / tile_width) + 1;
                    for (int i = 0; i < colmax; i++) {
                        if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, p->state.client_server_state.x + colbox.x + (i * tile_width), newy - colbox.y - colbox.height, 0, 0)) {
                            newy = (tiley + 1) * tile_height + colbox.y + colbox.height;
                            p->state.client_server_state.jump_accel_y = 0.0f;
                            p->state.client_server_state.accel_y = 0.0f;
                            p->state.client_state.flags &= ~PlayerClientFlagFalling;
                            bailout = true;
                            break;
                        }
                    }
                    if (bailout) {
                        break;
                    }

                    /* top edge right point */
                    if (collide_with_tile(TestTypeNormal, p, p->last_falling_y_pos, p->state.client_server_state.x + colbox.x + colbox.width - 1.0f, newy - colbox.y - colbox.height, 0, 0)) {
                        newy = (tiley + 1) * tile_height + colbox.y + colbox.height;
                        p->state.client_server_state.jump_accel_y = 0.0f;
                        p->state.client_server_state.accel_y = 0.0f;
                        p->state.client_state.flags &= ~PlayerClientFlagFalling;
                        break;
                    }
                }

                /* test if falling only */
                TestType test_type;
                if (move_down) {
                    p->last_falling_y_pos = Player::PlayerFallingTestMaxY;
                    test_type = TestTypeFallingThrough;
                } else {
                    test_type = TestTypeFalling;
                }
                if (movy > 0.0f) {
                    /* bottom edge */
                    bool found = false;
                    colmax = ((colbox.width - 1) / tile_width) + 1;
                    for (int i = 0; i < colmax; i++) {
                        double tile_friction = 0.0f;
                        if (collide_with_tile(test_type, p, p->last_falling_y_pos, p->state.client_server_state.x + colbox.x + (i * tile_width), newy - colbox.y, &tile_friction, 0)) {
                            if (!found) {
                                if (p->state.client_server_state.accel_y > YVeloLanding) {
                                    p->state.client_state.flags |= PlayerClientFlagLanded;
                                }
                                newy = tiley * tile_height + colbox.y;
                                p->state.client_server_state.jump_accel_y = 0.0f;
                                p->state.client_server_state.accel_y = 0.0f;
                                p->state.client_state.flags &= ~PlayerClientFlagFalling;
                                found = true;
                            }
                        }
                        if (tile_friction > ground_friction) {
                            ground_friction = tile_friction;
                        }
                    }

                    /* bottom edge right point */
                    double tile_friction = 0.0f;
                    if (collide_with_tile(test_type, p, p->last_falling_y_pos, p->state.client_server_state.x + colbox.x + colbox.width - 1.0f, newy - colbox.y, &tile_friction, 0)) {
                        if (!found) {
                            if (p->state.client_server_state.accel_y > YVeloLanding) {
                                p->state.client_state.flags |= PlayerClientFlagLanded;
                            }
                            newy = tiley * tile_height + colbox.y;
                            p->state.client_server_state.jump_accel_y = 0.0f;
                            p->state.client_server_state.accel_y = 0.0f;
                            p->state.client_state.flags &= ~PlayerClientFlagFalling;
                        }
                    }
                    if (tile_friction > ground_friction) {
                        ground_friction = tile_friction;
                    }
                }
            } while (false);

            /* save last falling y after y correction for falling tile collision tests */
            if (static_cast<int>(newy) > p->last_falling_y_pos) {
                p->last_falling_y_pos = static_cast<int>(newy);
            }

            /* if player was killed by tile, continue to next player here */
            if (p->state.server_state.flags & PlayerServerFlagDead) {
                continue;
            }

            /* collision with object and NPC */
            if (server) {
                CollisionBox p_colbox = colbox;
                p_colbox.x += static_cast<int>(newx);
                p_colbox.y = static_cast<int>(newy) - p_colbox.height - p_colbox.y;

                /* object collision? */
                for (GameObjects::iterator oit = game_objects.begin();
                    oit != game_objects.end(); oit++)
                {
                    GameObject *obj = *oit;
                    if (!obj->picked) {
                        TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();

                        CollisionBox obj_colbox = obj->object->get_colbox();
                        obj_colbox.x += static_cast<int>(obj->state.x);
                        obj_colbox.y = static_cast<int>(obj->state.y) + tg->get_height() - obj_colbox.height - obj_colbox.y;

                        /* intersection? */
                        if (p_colbox.intersects(obj_colbox)) {
                            bool proceed = false;
                            if (!pick_item(p, obj)) {
                                proceed = Tournament::pick_item(p, obj);
                            } else {
                                proceed = true;
                            }
                            if (proceed) {
                                GPickObject *po = new GPickObject;
                                po->id = obj->state.id;
                                po->to_net();
                                add_state_response(GPCPickObject, sizeof(GPickObject), po);
                                obj->picked = true;
                                obj->delete_me = obj->object->is_spawnable();
                            }
                        }
                    }
                }

                /* delete marked objects */
                game_objects.erase(std::remove_if(game_objects.begin(),
                    game_objects.end(), erase_element<GameObject>),
                    game_objects.end());

                /* NPC collision */
                for (SpawnableNPCs::iterator nit = spawnable_npcs.begin();
                    nit != spawnable_npcs.end(); nit++)
                {
                    SpawnableNPC *npc = *nit;

                    CollisionBox npc_colbox = npc->npc->get_colbox();
                    npc_colbox.x += static_cast<int>(npc->state.x);
                    npc_colbox.y = static_cast<int>(npc->state.y) -
                        npc_colbox.height - npc_colbox.y;

                    /* intersection? */
                    if (p_colbox.intersects(npc_colbox)) {
                        player_npc_collision(p, npc);
                    }
                }

                /* delete marked objects */
                remove_marked_npcs();
            }

            /* if player is sliding -> more friction */
            if (!move_left && !move_right && !(p->state.client_state.flags & PlayerClientFlagFalling)) {
                if (p->state.client_server_state.accel_x > -Epsilon && p->state.client_server_state.accel_x < Epsilon) {
                    p->state.client_server_state.accel_x = 0.0f;
                } else if (p->state.client_server_state.accel_x < -Epsilon) {
                    p->state.client_server_state.accel_x += (ground_friction * period_f);
                    if (p->state.client_server_state.accel_x > -Epsilon) {
                        p->state.client_server_state.accel_x = 0.0f;
                    }
                } else if (p->state.client_server_state.accel_x > Epsilon) {
                    p->state.client_server_state.accel_x -= (ground_friction * period_f);
                    if (p->state.client_server_state.accel_x < Epsilon) {
                        p->state.client_server_state.accel_x = 0.0f;
                    }
                }
            }

            /* fell off the screen */
            if (server) {
                if (newy - 100 > map_height * tile_height) {
                    player_dies(p, I18N_TNMT_PLAYER_FELL_OFF, p->get_player_name().c_str());
                    if (logger) {
                        logger->log(ServerLogger::LogTypeKill, i18n(I18N_TNMT_PLAYER_FELL_OFF, p->get_player_name().c_str()), p, p, "void");
                    }
                }
            }

            /* update player position */
            p->state.client_server_state.x = newx;
            p->state.client_server_state.y = newy;

            /* setup character icon and its animation */
            unsigned char icon = p->state.client_state.icon;
            p->animation_counter += period_f * AnimationMultiplier;
            if (p->state.client_state.flags & PlayerClientFlagDoShotAnimation) {
                if (p->state.client_state.flags & PlayerClientFlagOneShotFinished) {
                    p->state.client_state.flags &= ~PlayerClientFlagDoShotAnimation;
                }
                icon = static_cast<unsigned char>(CharacterAnimationShooting);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
                if (move_right && !move_left) {
                    p->state.client_server_state.direction = DirectionRight;
                }
                if (move_left && !move_right) {
                    p->state.client_server_state.direction = DirectionLeft;
                }
            } else if (p->state.client_server_state.jump_accel_y < -YVeloJumpToLanding) {
                icon = static_cast<unsigned char>(CharacterAnimationJumping);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
                if (move_right && !move_left) {
                    p->state.client_server_state.direction = DirectionRight;
                }
                if (move_left && !move_right) {
                    p->state.client_server_state.direction = DirectionLeft;
                }
            } else if (p->state.client_state.flags & PlayerClientFlagFalling) {
                icon = static_cast<unsigned char>(CharacterAnimationFalling);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
                if (move_right && !move_left) {
                    p->state.client_server_state.direction = DirectionRight;
                }
                if (move_left && !move_right) {
                    p->state.client_server_state.direction = DirectionLeft;
                }
            } else if (move_right && p->state.client_server_state.accel_x < Epsilon) {
                icon = static_cast<unsigned char>(CharacterAnimationSliding);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
            } else if (move_left && p->state.client_server_state.accel_x > -Epsilon) {
                icon = static_cast<unsigned char>(CharacterAnimationSliding);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
            } else if (move_right || move_left) {
                icon = static_cast<unsigned char>(CharacterAnimationRunning);
                if (icon != p->state.client_state.icon) {
                    p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                    p->animation_counter = 0.0f;
                }
                p->state.client_state.icon = icon;
                if (move_right && !move_left) {
                    p->state.client_server_state.direction = DirectionRight;
                }
                if (move_left && !move_right) {
                    p->state.client_server_state.direction = DirectionLeft;
                }
            } else {
                if (p->state.client_state.flags & PlayerClientFlagLanded) {
                    if (p->state.client_state.flags & PlayerClientFlagOneShotFinished) {
                        p->state.client_state.flags &= ~PlayerClientFlagLanded;
                    }
                    icon = static_cast<unsigned char>(CharacterAnimationLanding);
                    if (icon != p->state.client_state.icon) {
                        p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                        p->animation_counter = 0.0f;
                    }
                    p->state.client_state.icon = icon;
                } else if (p->state.client_server_state.accel_x < -Epsilon || p->state.client_server_state.accel_x > Epsilon) {
                    icon = static_cast<unsigned char>(CharacterAnimationSliding);
                    if (icon != p->state.client_state.icon) {
                        p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                        p->animation_counter = 0.0f;
                    }
                    p->state.client_state.icon = icon;
                } else {
                    icon = static_cast<unsigned char>(CharacterAnimationStanding);
                    if (icon != p->state.client_state.icon) {
                        p->state.client_state.flags &= ~PlayerClientFlagOneShotFinished;
                        p->animation_counter = 0.0f;
                    }
                    p->state.client_state.icon = icon;
                    if (move_right && !move_left) {
                        p->state.client_server_state.direction = DirectionRight;
                    }
                    if (move_left && !move_right) {
                        p->state.client_server_state.direction = DirectionLeft;
                    }
                }
            }

            Tile *t = p->get_characterset()->get_tile(DirectionLeft,
                static_cast<CharacterAnimation>(p->state.client_state.icon));

            if (p->animation_counter >= static_cast<double>(t->get_animation_speed())) {
                p->animation_counter = 0.0f;
                int index = p->state.client_state.iconindex + 1;
                int sz = static_cast<int>(t->get_tilegraphic()->get_tile_count());
                if (index >= sz) {
                    if (t->is_one_shot()) {
                        index = sz - 1;
                        p->state.client_state.flags |= PlayerClientFlagOneShotFinished;
                    } else {
                        index = 0;
                    }
                }
                p->state.client_state.iconindex = static_cast<unsigned char>(index);
            }

            /* check attacks */
            if (p == me) {
                check_attack(p, colbox, move_up, move_down, following_player);
            }
        }
    }

    /* post actions */
    players_post_actions();

    /* set top/left */
    if (following_player) {
        const CollisionBox& colbox = Characterset::Colbox;
        spectator_x = following_player->state.client_server_state.x + colbox.x + colbox.width / 2;
        spectator_y = following_player->state.client_server_state.y;
    }

    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    int origin_x = static_cast<int>(spectator_x);
    int origin_y = static_cast<int>(spectator_y);

    if (map_width * tile_width < view_width) {
        left = (view_width / 2) - (map_width * tile_width / 2);
    } else {
        left = -(origin_x - (view_width / 2));
        if (left > 0) left = 0;
        if (left < -((map_width * tile_width) - view_width)) {
            left = -((map_width * tile_width) - view_width);
        }
    }

    if (map_height * tile_height < view_height) {
        top = (view_height / 2) - (map_height * tile_height / 2);
    } else {
        top = -(origin_y - (view_height / 2));
        if (top > 0) top = 0;
        if (top < -((map_height * tile_height) - view_height)) {
            top = -((map_height * tile_height) - view_height);
        }
    }
}

void Tournament::subintegrate(ns_t ns) { }

void Tournament::players_post_actions() { }

void Tournament::round_finished_set_time(Player *p, GTransportTime *race) { }

bool Tournament::collide_with_tile(TestType type, Player *p,
    int last_falling_y_pos, double x, double y, double *friction, bool *killing)
{
    tilex = static_cast<int>(x) / tile_width;
    tiley = static_cast<int>(y) / tile_height;

    if (tilex < 0 || tiley < 0) {
        return false;
    }

    if (tilex > map_width - 1 || tiley > map_height - 1) {
        return false;
    }

    int index = map_array[tiley][tilex];
    if (index > -1) {
        Tile *t = tileset->get_tile(index);
        if (friction) {
            *friction = t->get_friction();
        }
        return tile_collision(type, p, last_falling_y_pos, t, killing);
    }

    return false;
}

bool Tournament::tile_collision(TestType type, Player *p,
    int last_falling_y_pos, Tile *t, bool *killing)
{
    if (killing) *killing = false;

    Tile::TileType tt = t->get_tile_type();
    switch (tt) {
        case Tile::TileTypeBaseRed:
        case Tile::TileTypeBaseBlue:
        case Tile::TileTypeBlocking:
            return true;

        case Tile::TileTypeKilling:
        {
            if (killing) *killing = true;
            if (server && p && p->is_alive_and_playing()) {
                p->state.server_state.score--;
                player_dies(p, I18N_TNMT_PLAYER_SUICIDE, p->get_player_name().c_str());
                if (logger) {
                    logger->log(ServerLogger::LogTypeKill, i18n(I18N_TNMT_PLAYER_SUICIDE, p->get_player_name().c_str()), p, p, "tile");
                }
            }
            break;
        }

        case Tile::TileTypeFallingOnlyBlocking:
        case Tile::TileTypeFallingOnlyBlockingNoDescending:
        case Tile::TileTypeFallingOnlyBlockingSingle:
            switch (type) {
                case TestTypeNormal:
                    return false;

                case TestTypeFallingThrough:
                    return (tt == Tile::TileTypeFallingOnlyBlockingNoDescending);

                case TestTypeFalling:
                {
                    bool test = (last_falling_y_pos <= tiley * tile_height);
                    if (tiley > 0) {
                        int index = map_array[tiley - 1][tilex];
                        if (index == -1 ||
                            tileset->get_tile(index)->get_tile_type() == Tile::TileTypeNonblocking ||
                            tileset->get_tile(index)->get_tile_type() == Tile::TileTypeHillZone)
                        {
                            return test;
                        } else {
                            return (tt == Tile::TileTypeFallingOnlyBlockingSingle ? test : false);
                            return false;
                        }
                    }

                    return test;
                }
            }
            return true;

        default:
            break;
    }

    return false;
}
