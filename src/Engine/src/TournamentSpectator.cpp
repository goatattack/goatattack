#include "Tournament.hpp"

const double SpectatorAccel = 0.2f;
const double SpectatorDecel = 0.05f;
const double SpecatorMaxAccel = 5.0f;

void Tournament::control_spectator(Player *me, double period_f) {
    bool move_left = false;
    bool move_right = false;
    bool move_up = false;
    bool move_down = false;

    if (me) {
        if (!me->is_alive_and_playing()) {
            move_left = ((me->state.client_server_state.key_states & PlayerKeyStateLeft) != 0) |
                ((me->state.client_server_state.jaxis & PlayerKeyStateLeft) != 0);

            move_right = ((me->state.client_server_state.key_states & PlayerKeyStateRight) != 0) |
                ((me->state.client_server_state.jaxis & PlayerKeyStateRight) != 0);

            move_up = ((me->state.client_server_state.key_states & PlayerKeyStateUp) != 0) |
                ((me->state.client_server_state.jaxis & PlayerKeyStateUp) != 0);

            move_down = ((me->state.client_server_state.key_states & PlayerKeyStateDown) != 0) |
                ((me->state.client_server_state.jaxis & PlayerKeyStateDown) != 0);

            /* horizontal acceleration */
            if (move_left) {
                spectator_accel_x -= (SpectatorAccel * period_f);
            }

            if (move_right) {
                spectator_accel_x += (SpectatorAccel * period_f);
            }

            if (spectator_accel_x < -SpecatorMaxAccel) {
                spectator_accel_x = -SpecatorMaxAccel;
            }

            if (spectator_accel_x > SpecatorMaxAccel) {
                spectator_accel_x = SpecatorMaxAccel;
            }

            /* vertical acceleration */
            if (move_up) {
                spectator_accel_y -= (SpectatorAccel * period_f);
            }

            if (move_down) {
                spectator_accel_y += (SpectatorAccel * period_f);
            }

            if (spectator_accel_y < -SpecatorMaxAccel) {
                spectator_accel_y = -SpecatorMaxAccel;
            }

            if (spectator_accel_y > SpecatorMaxAccel) {
                spectator_accel_y = SpecatorMaxAccel;
            }
        } else {
            spectator_accel_x = 0.0f;
            spectator_accel_y = 0.0f;
        }
    } else {
        spectator_accel_x = 0.0f;
        spectator_accel_y = 0.0f;
    }

    /* horizontal deceleration */
    if (!move_left && !move_right) {
        if (spectator_accel_x > -Epsilon && spectator_accel_x < Epsilon) {
            spectator_accel_x = 0.0f;
        } else if (spectator_accel_x < -Epsilon) {
            spectator_accel_x += (SpectatorDecel * period_f);
            if (spectator_accel_x > -Epsilon) {
                spectator_accel_x = 0.0f;
            }
        } else if (spectator_accel_x > Epsilon) {
            spectator_accel_x -= (SpectatorDecel * period_f);
            if (spectator_accel_x < Epsilon) {
                spectator_accel_x = 0.0f;
            }
        }
    }

    /* vertical deceleration */
    if (!move_up && !move_down) {
        if (spectator_accel_y > -Epsilon && spectator_accel_y < Epsilon) {
            spectator_accel_y = 0.0f;
        } else if (spectator_accel_y < -Epsilon) {
            spectator_accel_y += (SpectatorDecel * period_f);
            if (spectator_accel_y > -Epsilon) {
                spectator_accel_y = 0.0f;
            }
        } else if (spectator_accel_y > Epsilon) {
            spectator_accel_y -= (SpectatorDecel * period_f);
            if (spectator_accel_y < Epsilon) {
                spectator_accel_y = 0.0f;
            }
        }
    }

    /* new position */
    spectator_x += spectator_accel_x * period_f;
    spectator_y += spectator_accel_y * period_f;

    /* boundary checks */
    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();

    if (spectator_x < view_width / 2) {
        spectator_x = view_width / 2;
        spectator_accel_x = 0.0f;
    } else if (spectator_x > map_width * tile_width - view_width / 2) {
        spectator_x = map_width * tile_width - view_width / 2;
        spectator_accel_x = 0.0f;
    }

    if (spectator_y < view_height / 2) {
        spectator_y = view_height / 2;
        spectator_accel_y = 0.0f;
    } else if (spectator_y > map_height * tile_height - view_height / 2) {
        spectator_y = map_height * tile_height - view_height / 2;
        spectator_accel_y = 0.0f;
    }
}
