#include "Tournament.hpp"

#include <cstdlib>
#include <cmath>

void Tournament::draw() {
    if (ready) {
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
        subsystem.draw_icon(hud_ammo, 590, 10);
        subsystem.draw_text(fnt, 610, 10, buffer);

        sprintf(buffer, "%d", p->state.server_state.grenades);
        subsystem.draw_icon(hud_grenades, 590, 26);
        subsystem.draw_text(fnt, 610, 26, buffer);

        sprintf(buffer, "%d", p->state.server_state.bombs);
        subsystem.draw_icon(hud_bombs, 590, 42);
        subsystem.draw_text(fnt, 610, 42, buffer);

        if (has_frogs) {
            sprintf(buffer, "%d", p->state.server_state.frogs);
            subsystem.draw_icon(hud_frogs, 590, 58);
            subsystem.draw_text(fnt, 610, 58, buffer);
        }

        subsystem.reset_color();
    }

    /* draw tournament icon */
    if (tournament_icon) {
        subsystem.set_color(1.0f, 1.0f, 1.0f, 0.75f);
        TileGraphic *tg = tournament_icon->get_tile()->get_tilegraphic();
        int width = tg->get_width();
        int height = tg->get_height();
        subsystem.draw_icon(tournament_icon, view_width - width - 5, view_height - height - 25);
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
        std::string sptxt("SPECTATING");
        int tw = fnt->get_text_width(sptxt);
        int x = view_width - time_width - tw - 10;
        int y = view_height - fnt->get_font_height();
        subsystem.draw_text(fnt, x, y, sptxt);
    }

    /* warmup */
    if (warmup) {
        if (gui && gui->get_blink_on()) {
            std::string txt("WARM UP");
            int tw = fnt->get_text_width(txt);
            int x = view_width / 2 - tw / 2;
            int y = 50;
            subsystem.draw_text(fnt, x, y, txt);
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
            subsystem.reset_color();

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
                const CollisionBox& colbox = p->get_characterset()->get_colbox();
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

void Tournament::draw_statistics() {

}

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
                const CollisionBox& colbox = p->get_characterset()->get_damage_colbox();
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
