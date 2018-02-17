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

void Tournament::draw() {
    if (ready) {
        if (game_state.flags & GameStateFlagLobby) {
            draw_lobby();
        } else {
            int left_save = left;
            int top_save = top;
            if (screen_shaker) {
                left = left - (screen_shaker / 2) + (rand() % screen_shaker);
                top = top - (screen_shaker / 2) + (rand() % screen_shaker);
                screen_shaker--;
            }

            draw_background();
            draw_decoration();
            draw_lightmaps();
            draw_map(true);
            draw_animations(true);
            draw_objects();
            draw_object_addons();
            draw_npcs();
            draw_players();
            draw_player_addons();
            draw_map(false);
            draw_animations(false);
            draw_enemies_on_hud();
            draw_player_names();
            draw_team_colours();
            draw_text_animations();
            draw_score();
            draw_hud();
            if (show_statistics) {
                draw_statistics();
            }

            left = left_save;
            top = top_save;
        }
    }
}

void Tournament::draw_background() {
    if (background) {
        int layers = background->get_layer_count();
        int view_width = subsystem.get_view_width();
        int view_height = subsystem.get_view_height();
        subsystem.set_color(1.0f, 1.0f, 1.0f, background->get_alpha());
        for (int i = 0; i < layers; i++) {
            int parallax = parallax_shift + (layers - (i + 1));
            TileGraphic *tg = background->get_tilegraphic(i);
            int bg_width = tg->get_width();
            int bg_height = tg->get_height();

            int mh = map_height * tile_height;
            double p = mh - -top - view_height;
            double z = 1.0f - ((p / mh) / static_cast<double>(layers - i));
            int yo = bg_height - view_height;
            int y = static_cast<int>(yo * z);

            int x = (left / parallax) % bg_width;
            for (;;) {
                subsystem.draw_tilegraphic(tg, x, -y);
                x += bg_width;
                if (x > view_width) {
                    break;
                }
            }
        }
        subsystem.reset_color();
    }
}

void Tournament::draw_decoration() {
    int tx, ty;
    int cx, cy;

    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();

    ty = -top / tile_height;
    cy = top % tile_height;

    float dbr = static_cast<float>(decoration_brightness);
    subsystem.set_color(dbr, dbr, dbr, 1.0f);
    while (true) {
        tx = -left / tile_width;
        cx = left % tile_width;
        while (true) {
            if (tx >= 0 && ty >= 0) {
                if (tx >= map_width || ty >= map_height) {
                    break;
                }
                short index = decoration_array[ty][tx];
                if (index > -1) {
                    Tile *tile = tileset->get_tile(index);
                    TileGraphic *tg = tile->get_tilegraphic();
                    subsystem.draw_tilegraphic(tg, cx, cy);
                }
            }
            /* next tile */
            cx += tile_width;
            if (cx >= view_width) {
                break;
            }
            tx++;
        }
        cy += tile_height;
        if (cy >= view_height) {
            break;
        }
        ty++;
    }
    subsystem.reset_color();
}

void Tournament::draw_map(bool background) {
    int tx, ty;
    int cx, cy;

    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();

    ty = -top / tile_height;
    cy = top % tile_height;

    while (true) {
        tx = -left / tile_width;
        cx = left % tile_width;
        while (true) {
            if (tx >= 0 && ty >= 0) {
                if (tx >= map_width || ty >= map_height) {
                    break;
                }
                short index = map_array[ty][tx];
                if (index > -1) {
                    Tile *tile = tileset->get_tile(index);
                    if (tile->is_background() == background) {
                        TileGraphic *tg = tile->get_tilegraphic();
                        subsystem.draw_tilegraphic(tg, cx, cy);
                    }
                }
            }
            /* next tile */
            cx += tile_width;
            if (cx >= view_width) {
                break;
            }
            tx++;
        }
        cy += tile_height;
        if (cy >= view_height) {
            break;
        }
        ty++;
    }
}

void Tournament::draw_lightmaps() {
    if (lightmap) {
        int tx, ty;
        int cx, cy;

        int view_width = subsystem.get_view_width();
        int view_height = subsystem.get_view_height();


        ty = -top / LightMapSize;
        cy = top % LightMapSize;

        subsystem.set_color(1.0f, 1.0f, 1.0f, lightmap->get_alpha());
        while (true) {
            tx = -left / LightMapSize;
            cx = left % LightMapSize;
            while (true) {
                if (tx >= 0 && ty >= 0) {
                    Tile *tile = lightmap->get_tile(tx, ty);
                    if (tile) {
                        TileGraphic *tg = tile->get_tilegraphic();
                        subsystem.draw_tilegraphic(tg, cx, cy);
                    }
                }
                /* next tile */
                cx += LightMapSize;
                if (cx >= view_width) {
                    break;
                }
                tx++;
            }
            cy += LightMapSize;
            if (cy >= view_height) {
                break;
            }
            ty++;
        }
        subsystem.reset_color();
    }
}

void Tournament::draw_animations(bool background) {
    for (GameAnimations::iterator it = game_animations.begin();
        it != game_animations.end(); it++)
    {
        GameAnimation *gani = *it;
        if (gani->animation->is_in_background() == background) {
            int x = static_cast<int>(round(gani->state.x));
            int y = static_cast<int>(round(gani->state.y));

            TileGraphic *tg = gani->animation->get_tile()->get_tilegraphic();

            subsystem.draw_tilegraphic(tg, gani->index, x + left, y + top);

            if (debug) {
                if (gani->animation->get_damage()) {
                    int w = tg->get_width();
                    int h = tg->get_height();
                    subsystem.set_color(1.0f, 0.0f, 0.0f, 0.5f);
                    subsystem.draw_box(x + left, y + top, w, h);
                    subsystem.reset_color();
                }
            }
        }
    }
}

void Tournament::draw_objects() {
    for (GameObjects::iterator it = game_objects.begin();
        it != game_objects.end(); it++)
    {
        GameObject *obj = *it;
        if (!obj->picked) {
            Object::ObjectType type = obj->object->get_type();
            if (type != Object::ObjectTypeSpawnPointRed &&
                type != Object::ObjectTypeSpawnPointBlue &&
                type != Object::ObjectTypeSpawnPointFrog)
            {
                int x = static_cast<int>(round(obj->state.x));
                int y = static_cast<int>(round(obj->state.y));

                TileGraphic *tg = obj->object->get_tile()->get_tilegraphic();
                subsystem.draw_tilegraphic(tg, x + left, y + top);

                if (debug) {
                    const CollisionBox& colbox = obj->object->get_colbox();
                    subsystem.set_color(1.0f, 1.0f, 1.0f, 0.5f);
                    subsystem.draw_box(x + left + colbox.x,
                        y + top + tg->get_height() - colbox.height - colbox.y,
                        colbox.width, colbox.height);
                    subsystem.reset_color();
                }
            }
        }
    }
}

void Tournament::draw_text_animations() {
    for (GameTextAnimations::iterator it = game_text_animations.begin();
        it != game_text_animations.end(); it++)
    {
        GameTextAnimation *gani = *it;
        subsystem.draw_text(gani->font, gani->x + left, gani->y + top, gani->text);
    }
}

void Tournament::draw_hud() {
    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    char buffer[64];
    Font *fnt = resources.get_font("big");

    /* draw hud */
    Player *p = get_following_player();
    if (p) {
        subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);

        draw_lives_armor(p->state.server_state.health, lives_full, lives_half, lives_empty, 10);
        draw_lives_armor(p->state.server_state.armor, shield_full, shield_half, shield_empty, 26);

        sprintf(buffer, "%d", p->state.server_state.ammo);
        subsystem.draw_icon(hud_ammo, 585, 10);
        subsystem.draw_text(fnt, 605, 10, buffer);

        sprintf(buffer, "%d", p->state.server_state.grenades);
        subsystem.draw_icon(hud_grenades, 585, 26);
        subsystem.draw_text(fnt, 605, 26, buffer);

        sprintf(buffer, "%d", p->state.server_state.bombs);
        subsystem.draw_icon(hud_bombs, 585, 42);
        subsystem.draw_text(fnt, 605, 42, buffer);

        if (has_frogs) {
            sprintf(buffer, "%d", p->state.server_state.frogs);
            subsystem.draw_icon(hud_frogs, 585, 58);
            subsystem.draw_text(fnt, 605, 58, buffer);
        }

        subsystem.reset_color();
    }

    /* bottom up */
    int bottom_up_y = view_height;

    /* draw tournament icon */
    if (tournament_icon) {
        subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);
        TileGraphic *tg = tournament_icon->get_tile()->get_tilegraphic();
        int width = tg->get_width();
        int height = tg->get_height();
        bottom_up_y -= height + 25;
        subsystem.draw_icon(tournament_icon, view_width - width - 5, bottom_up_y); //view_height - height - 25);
        subsystem.reset_color();
    }


    /* draw lagometer */
    if (lagometer) {
        subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);
        TileGraphic *tg = lagometer->get_tilegraphic();
        int width = tg->get_width();
        int height = tg->get_height();
        bottom_up_y -= height + 5;
        subsystem.draw_tilegraphic(tg, view_width - width - 5, bottom_up_y);
        subsystem.reset_color();
    }

    /* draw remaining time */
    char *pb;
    std::string score;
    Tileset *ts = resources.get_tileset("digits");
    int tile_width = ts->get_tile(0)->get_tilegraphic()->get_width();
    int number_width = tile_width - 4;
    sprintf(buffer, "%d:%02d", game_state.seconds_remaining / 60, game_state.seconds_remaining % 60);
    int time_width = (((strlen(buffer) - 1) * number_width) + tile_width);
    int x = view_width - time_width - 3;
    int y = view_height - tile_width - 5;

    pb = buffer;
    while (*pb) {
        subsystem.draw_tile(ts->get_tile(*pb - '0'), x, y);
        x += number_width;
        pb++;
    }

    /* spectacting */
    if (p->state.server_state.flags & PlayerServerFlagSpectating) {
        std::string sptxt(i18n(I18N_TNMT_SPECTATING));
        int tw = fnt->get_text_width(sptxt);
        int x = view_width - time_width - tw - 10;
        int y = view_height - fnt->get_font_height() - 4;
        subsystem.draw_text(fnt, x, y, sptxt);
    }

    /* warmup */
    if (warmup) {
        if (gui && gui->get_tick_on()) {
            std::string txt(i18n(I18N_TNMT_WARMUP));
            int tw = fnt->get_text_width(txt);
            int x = view_width / 2 - tw / 2;
            int y = 50;
            subsystem.draw_text(fnt, x, y, txt);
        }
    }
}

void Tournament::draw_lobby() {
    Font *font_normal = resources.get_font("normal");
    Font *font_big = resources.get_font("big");
    int x = 120;
    int y = 10;
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = vw - x * 2;
    int wh = vh - y * 2;

    /* set alpha */
    float alpha = 0.8f;

    /* draw shadow */
    subsystem.set_color(0.0f, 0.0f, 0.0f, 0.2f);
    subsystem.draw_box(x + 7, y + 7, ww, wh);

    /* draw window */
    subsystem.set_color(0.75f, 0.5f, 0.75f, alpha);
    subsystem.draw_box(x, y, ww, wh);

    subsystem.set_color(0.25f, 0.0f, 0.25f, alpha);
    subsystem.draw_box(x + 1, y + 1, ww - 2, wh - 2);

    /* draw black bar */
    subsystem.set_color(0.0f, 0.0f, 0.0f, alpha);
    subsystem.draw_box(x + 10, y + 15, ww - 20, 21);

    /* reset */
    subsystem.reset_color();

    /* draw screws */
    subsystem.draw_icon(screw1, x + 5, y + 5);
    subsystem.draw_icon(screw2, vw - (x + 5) - 8, y + 5);
    subsystem.draw_icon(screw3, x + 5, vh - (y + 5) - 8);
    subsystem.draw_icon(screw4, vw - (x + 5) - 8, vh - (y + 5) - 8);

    /* draw title */
    std::string title(i18n(I18N_TNMT_LOBBY_TITLE));
    int tw = font_big->get_text_width(title);
    subsystem.draw_text(font_big, vw / 2 - tw / 2, y + 18, title);

    /* server info */
    std::string sn(server_name);
    Icon *flag = resources.get_flag_from_name(sn);
    int tx = x + Gui::Spc;
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_text(font_normal, tx, y + 40, i18n(I18N_TNMT_LOBBY_SERVER));
    subsystem.reset_color();
    Tile *preview = map.get_preview() ? map.get_preview() : resources.get_icon("map_preview")->get_tile();
    subsystem.draw_tile(preview, tx, y + 55);
    subsystem.draw_icon(resources.get_icon("map_border"), tx, y + 55);
    if (flag) {
        int flag_width = flag->get_tile()->get_tilegraphic()->get_width();
        subsystem.draw_icon(flag, tx + 64 + 5, y + 56 + 0 * 15);
        subsystem.draw_text(font_normal, tx + 64 + 5 + flag_width + 3, y + 55 + 0 * 15, sn);
    } else {
        subsystem.draw_text(font_normal, tx + 64 + 5, y + 55 + 0 * 15, sn);
    }
    subsystem.draw_text(font_normal, tx + 64 + 5, y + 55 + 1 * 15, map.get_description());
    subsystem.draw_text(font_normal, tx + 64 + 5, y + 55 + 2 * 15, map.get_name());
    subsystem.set_color(0.75f, 0.75f, 1.0f, 1.0f);
    subsystem.draw_text(font_normal, tx + 64 + 5, y + 55 + 3 * 15 + 6, get_game_type_name());
    subsystem.reset_color();

    /* players */
    int ty = y + 55 + 3 * 15 + 6 + 20;
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_text(font_normal, tx, ty, i18n(I18N_TNMT_LOBBY_PLAYERS));
    subsystem.reset_color();
    draw_lobby_players(tx, ty + 15, ww - (2 * Gui::Spc), wh - ty - 46);

    /* draw fire text */
    Player *me = get_me();
    if (!(me->state.server_state.flags & PlayerServerFlagIsReady)) {
        if (gui && gui->get_tick_on()) {
            std::string txt(is_team_tournament() ? i18n(I18N_TNMT_LOBBY_SELECT_START) : i18n(I18N_TNMT_LOBBY_START));
            int tw = font_big->get_text_width(txt);
            int x = vw / 2 - tw / 2;
            int y = vh - 42;
            subsystem.draw_text(font_big, x, y, txt);
        }
    }
}

void Tournament::draw_lobby_players(int x, int y, int w, int h) {
    float alpha = 0.8f;
    Font *font_normal = resources.get_font("normal");
    bool team_play = is_team_tournament();
    /* draw boxes */
    int m = w / 2;
    subsystem.set_color(0.15f, 0.15f, 0.15f, alpha);
    subsystem.draw_box(x, y, m - 5, h);
    subsystem.draw_box(x + m + 5, y, m - 5, h);
    subsystem.reset_color();

    /* draw player headers */
    int px = x + 2;
    int py = y + 2;
    Icon *thumb = resources.get_icon("thumb");
    Icon *team = resources.get_icon("ready_team");
    subsystem.set_color(1.0f, 1.0f, 0.0f, 1.0f);
    subsystem.draw_icon(thumb, px, py + 1);
    subsystem.draw_text(font_normal, px + 15, py, i18n(I18N_TNMT_SB_PLAYER));
    if (team_play) {
        subsystem.draw_icon(team, px + m - 35 - 15, py);
    }
    subsystem.draw_text(font_normal, px + m - 35, py, i18n(I18N_TNMT_SB_PING));
    subsystem.draw_icon(thumb, px + m + 5, py + 1);
    subsystem.draw_text(font_normal, px + m + 5 + 15, py, i18n(I18N_TNMT_SB_PLAYER));
    if (team_play) {
        subsystem.draw_icon(team, px + m + 5 + m - 35 - 15, py);
    }
    subsystem.draw_text(font_normal, px + m + 5 + m - 35, py, i18n(I18N_TNMT_SB_PING));
    subsystem.reset_color();

    /* draw players */
    char buffer[64];
    int tx = px;
    int ty = py + 15;
    Icon *yes = resources.get_icon("yes");
    Icon *no = resources.get_icon("no");
    Icon *team_red = resources.get_icon("ready_red");
    Icon *team_blue = resources.get_icon("ready_blue");
    int text_width = m - 20 - 35 - (team_play ? 15 : 0);
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        subsystem.draw_icon(p->state.server_state.flags & PlayerServerFlagIsReady ? yes : no, tx, ty);
        subsystem.draw_clipped_text(font_normal, tx + 15, ty, text_width, p->get_player_name());
        if (team_play) {
            subsystem.draw_icon((p->state.server_state.flags & PlayerServerFlagTeamRed ? team_red : team_blue), tx + m - 35 - 15, ty);
        }
        std::sprintf(buffer, "%d", p->state.server_state.ping_time);
        subsystem.draw_text(font_normal, tx + m - 35, ty, buffer);
        ty += font_normal->get_font_height();
        if (ty + font_normal->get_font_height() > y + h) {
            ty = py + 15;
            tx += m + 5;
        }
    }
}

void Tournament::draw_lives_armor(int amount, Icon *full, Icon *half, Icon *empty, int y) {
    int x = 495;
    //int x = 10;
    int available = amount / 20;
    for (int i = 0; i < 5; i++) {
        if (available > i) {
            subsystem.draw_icon(full, x, y);
        } else {
            if (available == i) {
                if (!i) {
                    if (amount) {
                        subsystem.draw_icon(half, x, y);
                    } else {
                        subsystem.draw_icon(empty, x, y);
                    }
                } else {
                    if (amount % (i * 20)) {
                        subsystem.draw_icon(half, x, y);
                    } else {
                        subsystem.draw_icon(empty, x, y);
                    }
                }
            } else {
                subsystem.draw_icon(empty, x, y);
            }
        }
        x += 16;
    }
}

void Tournament::draw_players() {
    subsystem.reset_color();
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->is_alive_and_playing()) {
            /* draw base character */
            Tile *t = p->get_characterset()->get_tile(
                static_cast<Direction>(p->state.client_server_state.direction),
                static_cast<CharacterAnimation>(p->state.client_state.icon));
            TileGraphic *tg = t->get_tilegraphic();
            int height = tg->get_height();
            subsystem.draw_tilegraphic(tg, p->state.client_state.iconindex,
                static_cast<int>(p->state.client_server_state.x) + left,
                static_cast<int>(p->state.client_server_state.y) + top - height);

            /* draw armor */
            if (p->state.server_state.armor) {
                t = p->get_characterset()->get_armor_overlay(
                    static_cast<Direction>(p->state.client_server_state.direction),
                    static_cast<CharacterAnimation>(p->state.client_state.icon));
                tg = t->get_tilegraphic();
                subsystem.draw_tilegraphic(tg, p->state.client_state.iconindex,
                    static_cast<int>(p->state.client_server_state.x) + left,
                        static_cast<int>(p->state.client_server_state.y) + top - height);
            }

            /* draw weapon belt */
            if (p->state.server_state.flags & PlayerServerFlagHasShotgunBelt) {
                t = p->get_characterset()->get_rifle_overlay(
                    static_cast<Direction>(p->state.client_server_state.direction),
                    static_cast<CharacterAnimation>(p->state.client_state.icon));
                tg = t->get_tilegraphic();
                subsystem.draw_tilegraphic(tg, p->state.client_state.iconindex,
                    static_cast<int>(p->state.client_server_state.x) + left,
                    static_cast<int>(p->state.client_server_state.y) + top - height);
            }

            /* draw afk */
            if (p->state.client_server_state.flags & PlayerClientServerFlagWriting) {
                TileGraphic *tg = player_afk->get_tile()->get_tilegraphic();
                int x = static_cast<int>(p->state.client_server_state.x) + left;
                int y = static_cast<int>(p->state.client_server_state.y) + top - height - tg->get_height();
                subsystem.draw_tilegraphic(tg, player_afk_index, x, y);
            }

            /* draw player name */
            if (player_configuration && player_configuration->get_show_player_name()) {
                if (p->font) {
                    int x = static_cast<int>(p->state.client_server_state.x) + (tg->get_width() / 2) - (p->player_name_width / 2);
                    int y = static_cast<int>(p->state.client_server_state.y) - (tg->get_height()) - 15;
                    subsystem.draw_text(p->font, x + left, y + top, p->get_player_name());
                }
            }

            /* draw collision box */
            if (debug) {
                const CollisionBox& colbox = Characterset::DamageColbox;
                subsystem.set_color(1.0f, 0.0f, 0.0f, 0.5f);
                subsystem.draw_box(static_cast<int>(p->state.client_server_state.x) + left + colbox.x,
                    static_cast<int>(p->state.client_server_state.y) + top - colbox.height - colbox.y,
                    colbox.width, colbox.height);
                subsystem.reset_color();
            }
        }
    }
}

void Tournament::draw_npcs() {
    for (SpawnableNPCs::iterator it = spawnable_npcs.begin();
        it != spawnable_npcs.end(); it++)
    {
        SpawnableNPC *npc = *it;

        Tile *t = npc->npc->get_tile(static_cast<Direction>(npc->state.direction),
            npc->icon);

        TileGraphic *tg = t->get_tilegraphic();
        int height = tg->get_height();

        subsystem.draw_tilegraphic(tg, npc->iconindex,
            static_cast<int>(npc->state.x) + left,
            static_cast<int>(npc->state.y) + top - height);

        subsystem.reset_color();

        /* draw collision box */
        if (debug) {
            const CollisionBox& colbox = npc->npc->get_damage_colbox();
            subsystem.set_color(1.0f, 0.0f, 0.0f, 0.5f);
            subsystem.draw_box(static_cast<int>(npc->state.x) + left + colbox.x,
                static_cast<int>(npc->state.y) + top - colbox.height - colbox.y,
                colbox.width, colbox.height);
            subsystem.reset_color();
        }
    }
}

void Tournament::draw_player_names() {
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->is_alive_and_playing()) {
            /* draw player name */
            Tile *t = p->get_characterset()->get_tile(
                static_cast<Direction>(p->state.client_server_state.direction),
                static_cast<CharacterAnimation>(p->state.client_state.icon));
            TileGraphic *tg = t->get_tilegraphic();
            if (player_configuration && player_configuration->get_show_player_name()) {
                if (p->font) {
                    int x = static_cast<int>(p->state.client_server_state.x) + (tg->get_width() / 2) - (p->player_name_width / 2);
                    int y = static_cast<int>(p->state.client_server_state.y) - (tg->get_height()) - 15;
                    subsystem.draw_text(p->font, x + left, y + top, p->get_player_name());
                }
            }
        }
    }
}

void Tournament::draw_score() { }

void Tournament::draw_object_addons() { }

void Tournament::draw_player_addons() { }

void Tournament::draw_team_colours() { }

void Tournament::draw_statistics() { }

void Tournament::draw_enemies_on_hud() {
    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    int ih = 8;
    int iw = 8;
    Player *me = get_me();

    subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p != me) {
            if (p->is_alive_and_playing()) {
                const CollisionBox& colbox = Characterset::DamageColbox;
                int cbw = colbox.width / 2;
                int cbh = colbox.height / 2;
                bool draw = false;
                int x = static_cast<int>(p->state.client_server_state.x) + left + colbox.x + cbw - iw / 2;
                int y = static_cast<int>(p->state.client_server_state.y) + top - colbox.y - cbh - ih / 2;
                if (x < 0) {
                    x = 0;
                    draw = true;
                }
                if (y < 0) {
                    draw = true;
                    y = 0;
                }
                if (x > view_width - ih) {
                    draw = true;
                    x = view_width - ih;
                }
                if (y > view_height - ih) {
                    draw = true;
                    y = view_height - ih;
                }

                if (draw) {
                    subsystem.draw_icon(enemy_indicator, x, y);
                }
            }
        }
    }
    subsystem.reset_color();
}
