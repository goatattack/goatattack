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

#include "MapEditor.hpp"
#include "Utils.hpp"
#include "Globals.hpp"
#include "Cargo.hpp"

#include <cstdlib>
#include <algorithm>

#include <iostream>

const double AnimationMultiplier = 3.0f;
static const ns_t ns_fc = 10000000;   /* for 0.01 s */
static const char *left_arrow = "\xe2\x86\x90";
static const char *right_arrow = "\xe2\x86\x92";

MapEditor::SelectRect::SelectRect() : width(0), height(0), decoration(0), map(0) {
    reset();
}

MapEditor::SelectRect::~SelectRect() {
    clear();
}

void MapEditor::SelectRect::reset() {
    select_index = 0;
}

void MapEditor::SelectRect::clear() {
    for (int i = 0; i < height; i++) {
        delete[] decoration[i];
        delete[] map[i];
    }
    delete[] decoration;
    delete[] map;
    decoration = 0;
    map = 0;
    objects.clear();
    lights.clear();
}

bool MapEditor::SelectRect::add_point(int x, int y) {
    px[select_index] = x;
    py[select_index] = y;
    if (select_index < 1) {
        select_index++;
        return false;
    }

    return true;
}

MapEditor::MapEditor(Resources& resources, Subsystem& subsystem, Configuration& config)
    : Gui(resources, subsystem, resources.get_font("normal")),
      resources(resources), subsystem(subsystem), i18n(subsystem.get_i18n()),
      config(config), running(true), top(0), left(0), selected_tile_index(0), selected_object_index(0),
      mouse_down_mode(0), move_map(false), wmap(new EditableMap(resources, subsystem)),
      center_icon(resources.get_icon("center")), mirrored_drawing(false),
      draw_tile_props(false), draw_tile_zorder(false), draw_tile_number(false),
      draw_even_only(false), drawing_decoration(false), draw_decoration_on_screen(true),
      draw_lightmap_on_screen(true), draw_map_on_screen(true), draw_objects_on_screen(true),
      draw_light_sources(true), draw_used_tiles(false), draw_mode(DrawModeTile),
      light_source(resources.get_icon("light_source")), compile_thread(0),
      home_workdir(get_home_directory() + dir_separator + UserDirectory),
      is_drawing_rect(true), is_selecting(false), use_selection(false), tile_selector_page(0)
{
    ts_picture = 0;
    os_picture = 0;
    create_toolbox();
    get_now(last);
}

MapEditor::~MapEditor() {
    if (wmap) {
        delete wmap;
    }
}

void MapEditor::idle() throw (Exception) {
    char buffer[64];

    /* update tiles for animation */
    get_now(now);
    double period_f = diff_ns(last, now) / static_cast<double>(ns_fc);
    last = now;
    if (wmap && wmap->get_tileset_ptr()) {
        resources.update_tile_index(period_f * AnimationMultiplier, wmap->get_tileset_ptr());
    }

    /* picture blinker */
    if (ts_picture) {
        ts_picture->set_visible(get_blink_on());
    }
    if (os_picture) {
        os_picture->set_visible(get_blink_on());
    }

    /* get mouse position */
    bool draw_sel_ok = false;
    int sx = 0;
    int sy = 0;
    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int w = ts->get_tile_width();
            int h = ts->get_tile_height();
            sx = get_mouse_x();
            sy = get_mouse_y();
            sx -= left;
            sy -= top;
            sx /= w;
            sy /= h;
            sx *= w;
            sy *= h;
            sx += left;
            sy += top;
            draw_sel_ok = true;
        }
    }

    /* draw stuff */
    draw_background();
    if (draw_decoration_on_screen) {
        draw_decoration();
    }

    if (draw_lightmap_on_screen) {
        draw_lightmap();
    }

    if (use_selection && draw_mode == DrawModeTile && drawing_decoration && draw_sel_ok) {
        draw_selection(sx, sy, true);
    }

    if (draw_map_on_screen) {
        draw_map(true);
    }

    if (draw_objects_on_screen) {
        draw_objects();
    }

    if (draw_map_on_screen) {
        draw_map(false);
    }

    if (draw_light_sources) {
        draw_lights();
    }

    if (use_selection && draw_mode == DrawModeObject && draw_sel_ok) {
        draw_selection_objects(sx, sy);
    }

    if (use_selection && draw_mode == DrawModeLightSources && draw_sel_ok) {
        draw_selection_lights(sx, sy);
    }

    if (use_selection && draw_mode == DrawModeTile && !drawing_decoration && draw_sel_ok) {
        draw_selection(sx, sy, false);
    }

    /* draw center cross */
    int x = subsystem.get_view_width() / 2;
    int y = subsystem.get_view_height() / 2;
    TileGraphic *tg = center_icon->get_tile()->get_tilegraphic();
    x -= tg->get_width() / 2;
    y -= tg->get_height() / 2;
    subsystem.draw_icon(center_icon, x, y);

    /* draw position marker */
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    sprintf(buffer, "x = %d, y = %d", -left + vw / 2, -top + vh / 2);
    subsystem.draw_text(get_font(), 5, 5, buffer);

    /* draw modified marker */
    if (wmap->is_modified()) {
        std::string txt(i18n(I18N_ME_MODIFIED));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, 5, txt);
    }

    /* mirrored drawing */
    if (mirrored_drawing) {
        std::string txt(i18n(I18N_ME_MIRRORING));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - f->get_font_height() - 5, txt);
    }

    /* drawing decoration */
    if (drawing_decoration) {
        std::string txt(i18n(I18N_ME_DECORATION));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 2 * f->get_font_height() - 5, txt);
    }

    /* decoration indicator */
    if (!draw_decoration_on_screen) {
        std::string txt(i18n(I18N_ME_NO_DEC));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 3 * f->get_font_height() - 5, txt);
    }

    /* lightmap indicator */
    if (!draw_lightmap_on_screen) {
        std::string txt(i18n(I18N_ME_NO_LGTMP));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 4 * f->get_font_height() - 5, txt);
    }

    /* map indicator */
    if (!draw_map_on_screen) {
        std::string txt(i18n(I18N_ME_NO_MP));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 5 * f->get_font_height() - 5, txt);
    }

    /* light indicator */
    if (!draw_objects_on_screen) {
        std::string txt(i18n(I18N_ME_NO_OBJ));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 6 * f->get_font_height() - 5, txt);
    }

    /* light indicator */
    if (!draw_light_sources) {
        std::string txt(i18n(I18N_ME_NO_LGT));
        Font *f = get_font();
        int tw = f->get_text_width(txt);
        subsystem.draw_text(f, subsystem.get_view_width() - tw - 5, vh - 7 * f->get_font_height() - 5, txt);
    }

    /* draw mode */
    std::string txt;
    switch (draw_mode) {
        case DrawModeTile:
            txt = i18n(I18N_ME_MODE_TILE);
            break;

        case DrawModeObject:
            txt = i18n(I18N_ME_MODE_OBJECT);
            break;

        case DrawModeLightSources:
            txt = i18n(I18N_ME_MODE_LIGHT);
            break;

        case DrawModeEditLight:
            txt = i18n(I18N_ME_MODE_EDIT_LIGHT);
            break;

        default:
            break;
    }
    Font *f = get_font();
    subsystem.draw_text(f, 5, subsystem.get_view_height() - f->get_font_height() - 5, txt);

    /* draw "select" */
    if (is_selecting) {
        subsystem.draw_text(f, 5, subsystem.get_view_height() - f->get_font_height() * 2 - 5, i18n(I18N_ME_SELECT));
    }

    /* draw object blinking */
    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            Object *obj = static_cast<Object *>(resources.get_objects()[selected_object_index].object);
            int w = ts->get_tile_width();
            int h = ts->get_tile_height();
            x = get_mouse_x();
            y = get_mouse_y();
            x -= left;
            y -= top;
            x /= w;
            y /= h;
            x *= w;
            y *= h;
            x += left;
            y += top;
            if (get_tick_on() && !use_selection) {
                if (draw_mode == DrawModeObject) {
                    subsystem.draw_tilegraphic(obj->get_tile()->get_tilegraphic(), x, y);
                } else if (draw_mode == DrawModeLightSources) {
                    subsystem.draw_icon(light_source, x, y);
                }
            }
            if (is_drawing_rect) {
                if (is_selecting && select_rect.select_index) {
                    x = select_rect.px[0] * w + left;
                    y = select_rect.py[0] * h + top;
                    int x2 = get_mouse_x();
                    int y2 = get_mouse_y();
                    x2 -= left;
                    y2 -= top;
                    x2 /= w;
                    y2 /= h;
                    x2 *= w;
                    y2 *= h;
                    x2 += left;
                    y2 += top;
                    if (x > x2) std::swap(x, x2);
                    if (y > y2) std::swap(y, y2);
                    w = x2 - x + w;
                    h = y2 - y + h;
                } else if (draw_mode == DrawModeObject) {
                    w = obj->get_tile()->get_tilegraphic()->get_width();
                    h = obj->get_tile()->get_tilegraphic()->get_height();
                }
                subsystem.set_color(1.0f, 1.0f, 1.0f, 0.85f);
                draw_rect(x, y, w, h);
                subsystem.reset_color();

            }
        }
    }

    /* compile finished? */
    if (compile_thread) {
        lbl_compile->set_caption(i18n(I18N_ME_COMPILING, compile_thread->get_percentage()));
        if (compile_thread->is_finished()) {
            finalise_lightmap();
        }
    }
}

void MapEditor::on_input_event(const InputData& input) {
    if (get_stack_count() == 1) {
        switch (input.data_type) {
            case InputData::InputDataTypeMouseLeftDown:
                if (use_selection) {
                    if (wmap) {
                        Tileset *ts = wmap->get_tileset_ptr();
                        if (ts) {
                            int w = ts->get_tile_width();
                            int h = ts->get_tile_height();
                            int x = input.param1 / subsystem.get_zoom_factor();
                            int y = input.param2 / subsystem.get_zoom_factor();
                            x -= left;
                            y -= top;
                            x /= w;
                            y /= h;
                            switch (draw_mode) {
                                case DrawModeTile:
                                    insert_selection(x, y, drawing_decoration);
                                    break;

                                case DrawModeObject:
                                    insert_selection_objects(x, y);
                                    break;

                                case DrawModeLightSources:
                                    insert_selection_lights(x, y);
                                    break;

                                default:
                                    /* do nothing */
                                    break;
                            }
                        }
                    }
                } else if (is_selecting) {
                    if (wmap) {
                        Tileset *ts = wmap->get_tileset_ptr();
                        if (ts) {
                            int w = ts->get_tile_width();
                            int h = ts->get_tile_height();
                            int x = input.param1 / subsystem.get_zoom_factor();
                            int y = input.param2 / subsystem.get_zoom_factor();
                            x -= left;
                            y -= top;
                            x /= w;
                            y /= h;
                            if (x < 0) x = 0;
                            if (x >= wmap->get_width()) x = wmap->get_width() - 1;
                            if (y < 0) y = 0;
                            if (y >= wmap->get_height()) y = wmap->get_height() - 1;
                            if (select_rect.add_point(x, y)) {
                                copy_selection();
                                select_rect.reset();
                            }
                        }
                    }
                } else {
                    mouse_down_mode = 1;
                    hand_draw(input.param1, input.param2);
                }
                break;

            case InputData::InputDataTypeMouseRightDown:
                if (!is_selecting) {
                    mouse_down_mode = 2;
                    hand_draw(input.param1, input.param2);
                }
                break;

            case InputData::InputDataTypeMouseMiddleDown:
                move_origin_x = input.param1 / subsystem.get_zoom_factor();
                move_origin_y = input.param2 / subsystem.get_zoom_factor();
                move_origin_left = left;
                move_origin_top = top;
                set_mousepointer(MousepointerMove);
                move_map = true;
                break;

            case InputData::InputDataTypeMouseMiddleUp:
                set_mousepointer(MousepointerDefault);
                move_map = false;
                break;

            case InputData::InputDataTypeMouseMove:
                if (move_map) {
                    mouse_move_map(input.param1, input.param2);
                } else if (mouse_down_mode) {
                    if (!is_selecting) {
                        hand_draw(input.param1, input.param2);
                    }
                }
                break;

            case InputData::InputDataTypeMouseLeftUp:
            case InputData::InputDataTypeMouseRightUp:
                mouse_down_mode = 0;
                break;

            case InputData::InputDataTypeKeyDown:
                switch (input.key_type) {
                    case InputData::InputKeyTypeShift:
                    {
                        if (!is_selecting) {
                            is_selecting = true;
                            select_rect.reset();
                        }
                        break;
                    }

                    case InputData::InputKeyTypeControl:
                    {
                        use_selection = true;
                        break;
                    }

                    default:
                    {
                        switch (input.keycode) {
                            case 9:
                                mode_selector_click();
                                break;

                            case '1':
                                draw_decoration_on_screen = !draw_decoration_on_screen;
                                break;

                            case '2':
                                draw_lightmap_on_screen = !draw_lightmap_on_screen;
                                break;

                            case '3':
                                draw_map_on_screen = !draw_map_on_screen;
                                break;

                            case '4':
                                draw_objects_on_screen = !draw_objects_on_screen;
                                break;

                            case '5':
                                draw_light_sources = !draw_light_sources;
                                break;

                            case 't':
                                tile_selector_click();
                                break;

                            case 'o':
                                object_selector_click();
                                break;

                            case 'c':
                                center_map();
                                break;

                            case 'm':
                                mirrored_drawing = !mirrored_drawing;
                                break;

                            case 'd':
                                drawing_decoration = !drawing_decoration;
                                break;

                            case 'p':
                                draw_tile_props = true;
                                break;

                            case 'z':
                                draw_tile_zorder = true;
                                break;

                            case 'n':
                                draw_tile_number = true;
                                break;

                            case 'e':
                                draw_even_only = true;
                                break;

                            case 'u':
                                draw_used_tiles = true;
                                break;
                        }
                        break;
                    }
                }
                break;

            case InputData::InputDataTypeKeyUp:
                switch (input.key_type) {
                    case InputData::InputKeyTypeShift:
                    {
                        is_selecting = false;
                        break;
                    }

                    case InputData::InputKeyTypeControl:
                    {
                        use_selection = false;
                        break;
                    }

                    default:
                    {
                        switch (input.keycode) {
                            case 'p':
                                draw_tile_props = false;
                                break;

                            case 'z':
                                draw_tile_zorder = false;
                                break;

                            case 'n':
                                draw_tile_number = false;
                                break;

                            case 'e':
                                draw_even_only = false;
                                break;

                            case 'u':
                                draw_used_tiles = false;
                                break;
                        }
                        break;
                    }
                }
                break;

            default:
                break;
        }
    }
}

void MapEditor::on_leave() { }

void MapEditor::hand_draw(int x, int y) {
    switch (draw_mode) {
        case DrawModeTile:
            place_tile(x, y, mouse_down_mode == 2);
            break;

        case DrawModeObject:
            place_object(x, y, mouse_down_mode == 2);
            break;

        case DrawModeLightSources:
            place_light(x, y, mouse_down_mode == 2);
            break;

        case DrawModeEditLight:
            mouse_down_mode = 0;
            edit_light(x, y);
            break;

        default:
            break;
    }
}

void MapEditor::create_toolbox() {
    int w = 18;
    int h = 18;
    int bh = h - 1;
    int ofsy = 4;
    int l = 0;
    GuiButton *btn;

    int wh = 262;
    GuiWindow *window = push_window(5, get_subsystem().get_view_height() / 2 - wh / 2, 26, wh, ""); //i18n(I18N_ME_TOOL));
    window->show_screws(false);

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_mode_selector_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_MODE_SELECTOR));
    create_picture(btn, 1, 1, resources.get_icon("me_select")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_center_map, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_CENTER_MAP));
    create_picture(btn, 1, 1, resources.get_icon("me_center")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_map_properties_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_MAP_PROPERTIES));
    create_picture(btn, 1, 1, resources.get_icon("me_properties")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_load_map_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_LOAD_MAP));
    create_picture(btn, 1, 1, resources.get_icon("me_load")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_save_map_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_SAVE_MAP));
    create_picture(btn, 1, 1, resources.get_icon("me_save")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_tile_selector_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_TILE_SELECTOR));
    create_picture(btn, 1, 1, resources.get_icon("me_tile")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_object_selector_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_OBJECT_SELECTOR));
    create_picture(btn, 1, 1, resources.get_icon("me_objects")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_hcopy_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_HORZ_COPY));
    create_picture(btn, 1, 1, resources.get_icon("me_hcopy")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_calculate_light_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_QUICK_LIGHTMAP));
    create_picture(btn, 1, 1, resources.get_icon("me_calclight")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_calculate_light_pixel_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_PIXEL_LIGHTMAP));
    create_picture(btn, 1, 1, resources.get_icon("me_calclight_pixel")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_pack_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_MAP_PACKAGE));
    create_picture(btn, 1, 1, resources.get_icon("me_pack")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_zap_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_ZAP));
    create_picture(btn, 1, 1, resources.get_icon("me_clean")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_options_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_OPTIONS));
    create_picture(btn, 1, 1, resources.get_icon("me_options")->get_tile()->get_tilegraphic());

    btn = create_button(window, 3, ofsy + (bh * l++), w, h, "", static_exit_click, this);
    btn->show_bolts(false);
    btn->set_tooltip_text(i18n(I18N_ME_QUIT));
    create_picture(btn, 1, 1, resources.get_icon("me_exit")->get_tile()->get_tilegraphic());
}

/* ************************************************************************** */
/* helper functions                                                           */
/* ************************************************************************** */
void MapEditor::static_window_close_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->pop_window();
}

GuiButton *MapEditor::add_close_button(GuiWindow *window, GuiVirtualButton::OnClick on_click, const char *button_text) {
    std::string btn_close(button_text ? button_text : i18n(I18N_BUTTON_CLOSE));
    int bw_close = get_font()->get_text_width(btn_close) + 24;

    int width = window->get_client_width();
    int height = window->get_client_height();
    int bh = 18;

    if (!on_click) {
        on_click = static_window_close_click;
    }

    return create_button(window, width / 2 - bw_close / 2, height - bh - Spc, bw_close, bh, btn_close, on_click, this);
}

void MapEditor::add_ok_cancel_buttons(GuiWindow *window, GuiVirtualButton::OnClick on_click) {
    std::string btn_cancel(i18n(I18N_BUTTON_CANCEL));
    int bw_cancel = get_font()->get_text_width(btn_cancel) + 24;

    std::string btn_ok(i18n(I18N_BUTTON_OK));
    int bw_ok = get_font()->get_text_width(btn_ok) + 24;

    int width = window->get_client_width();
    int height = window->get_client_height();
    int bh = 18;
    create_button(window, width - bw_cancel - Spc, height - bh - Spc, bw_cancel, bh, btn_cancel, static_window_close_click, this);
    create_button(window, width - bw_cancel - Spc - bw_ok - 5, height - bh - Spc, bw_ok, bh, btn_ok, on_click, this);
}

void MapEditor::add_ok_cancel_buttons(GuiWindow *window, GuiVirtualButton::OnClick on_ok_click,
    GuiVirtualButton::OnClick on_cancel_click)
{
    std::string btn_cancel(i18n(I18N_BUTTON_CANCEL));
    int bw_cancel = get_font()->get_text_width(btn_cancel) + 24;

    std::string btn_ok(i18n(I18N_BUTTON_OK));
    int bw_ok = get_font()->get_text_width(btn_ok) + 24;

    int width = window->get_client_width();
    int height = window->get_client_height();
    int bh = 18;
    create_button(window, width - bw_cancel - Spc, height - bh - Spc, bw_cancel, bh, btn_cancel, on_cancel_click, this);
    create_button(window, width - bw_cancel - Spc - bw_ok - 5, height - bh - Spc, bw_ok, bh, btn_ok, on_ok_click, this);
}

void MapEditor::draw_background() {
    Background *background = wmap->get_background_ptr();
    Tileset *tileset = wmap->get_tileset_ptr();
    if (background && tileset) {
        int layers = background->get_layer_count();
        int view_width = subsystem.get_view_width();
        int view_height = subsystem.get_view_height();
        subsystem.set_color(1.0f, 1.0f, 1.0f, background->get_alpha());
        for (int i = 0; i < layers; i++) {
            int parallax = wmap->get_parallax_shift() + (layers - (i + 1));
            TileGraphic *tg = background->get_tilegraphic(i);
            int bg_width = tg->get_width();
            int bg_height = tg->get_height();
            int mh = wmap->get_height() * tileset->get_tile_height();
            double p = mh - -top - view_height;
            //double z = 1.0f - p / mh;
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

void MapEditor::draw_decoration() {
    static char buffer[16];
    int tx, ty;
    int cx, cy;

    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    Tileset *tileset = wmap->get_tileset_ptr();

    if (tileset) {
        int tile_width = tileset->get_tile_width();
        int tile_height = tileset->get_tile_height();

        ty = -top / tile_height;
        cy = top % tile_height;
        ty--;
        cy -= tile_height;

        short **map_array = wmap->get_decoration();
        int width = wmap->get_width();
        int height = wmap->get_height();
        float brght = static_cast<float>(wmap->get_decoration_brightness());
        Font *f = get_font();
        int fh = f->get_font_height();

        subsystem.set_color(brght, brght, brght, 1.0f);
        while (true) {
            tx = -left / tile_width;
            cx = left % tile_width;
            tx--;
            cx -= tile_width;
            while (true) {
                if (tx >= 0 && ty >= 0) {
                    if (tx < width && ty < height) {
                        short index = map_array[ty][tx];
                        if (index > -1) {
                            Tile *tile = tileset->get_tile(index);
                            if (tile) {
                                TileGraphic *tg = tile->get_tilegraphic();
                                subsystem.draw_tilegraphic(tg, cx, cy);
                            }

                            if (draw_used_tiles) {
                                subsystem.draw_box(cx, cy, tile_width, tile_height);
                            }

                            if (draw_tile_number && drawing_decoration) {
                                int n = ty * width + tx;
                                if ((draw_even_only && n % 2 == 0) ||
                                    (!draw_even_only && n % 2))
                                {
                                    sprintf(buffer, "%d", index);
                                    int tw = f->get_text_width(buffer);
                                    subsystem.draw_text(f, cx + tile_width / 2 - tw / 2, cy + tile_height / 2 - fh / 2, buffer);
                                }
                            }

                        }
                    } else {
                        break;
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
}

void MapEditor::draw_lightmap() {
    if (wmap) {
        Lightmap *lightmap = wmap->get_lightmap();
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
}


void MapEditor::draw_map(bool background) {
    static char buffer[16];
    int tx, ty;
    int cx, cy;

    int view_width = subsystem.get_view_width();
    int view_height = subsystem.get_view_height();
    Tileset *tileset = wmap->get_tileset_ptr();

    if (tileset) {
        int tile_width = tileset->get_tile_width();
        int tile_height = tileset->get_tile_height();

        ty = -top / tile_height;
        cy = top % tile_height;
        ty--;
        cy -= tile_height;
        Font *f = get_font();
        int fh = f->get_font_height();

        short **map_array = wmap->get_map();
        int width = wmap->get_width();
        int height = wmap->get_height();

        while (true) {
            tx = -left / tile_width;
            cx = left % tile_width;
            tx--;
            cx -= tile_width;
            while (true) {
                if (tx >= 0 && ty >= 0) {
                    if (tx < width && ty < height) {
                        short index = map_array[ty][tx];
                        if (index > -1) {
                            Tile *tile = tileset->get_tile(index);
                            if (tile && tile->is_background() == background) {
                                TileGraphic *tg = tile->get_tilegraphic();
                                subsystem.draw_tilegraphic(tg, cx, cy);
                                if (draw_tile_props) {
                                    const char *p = "?";
                                    switch (tile->get_tile_type()) {
                                        case Tile::TileTypeNonblocking:
                                            p = "n";
                                            break;

                                        case Tile::TileTypeBlocking:
                                            p = "b";
                                            break;

                                        case Tile::TileTypeFallingOnlyBlocking:
                                            p = "f";
                                            break;

                                        case Tile::TileTypeBaseRed:
                                            p = "r";
                                            break;

                                        case Tile::TileTypeBaseBlue:
                                            p = "b";
                                            break;

                                        case Tile::TileTypeKilling:
                                            p = "k";
                                            break;

                                        default:
                                            break;
                                    }
                                    int cw = f->get_text_width(p);
                                    subsystem.draw_char(f, cx + tile_width / 2 - cw / 2, cy + tile_height / 2 - fh / 2, p);
                                }
                                if (draw_tile_zorder) {
                                    const char *p = (tile->is_background() ? "b" : "f");
                                    int cw = f->get_text_width(p);
                                    subsystem.draw_char(f, cx + tile_width / 2 - cw / 2, cy + tile_height / 2 - fh / 2, p);
                                }

                                if (draw_used_tiles) {
                                    subsystem.draw_box(cx, cy, tile_width, tile_height);
                                }

                                if (draw_tile_number && !drawing_decoration) {
                                    int n = ty * width + tx;
                                    if ((draw_even_only && n % 2 == 0) ||
                                        (!draw_even_only && n % 2))
                                    {
                                        sprintf(buffer, "%d", index);
                                        int tw = f->get_text_width(buffer);
                                        subsystem.draw_text(f, cx + tile_width / 2 - tw / 2, cy + tile_height / 2 - fh / 2, buffer);
                                    }
                                }
                            }
                        }
                    } else {
                        /* draw void */
                        subsystem.set_color(0.5f, 0.0f, 0.0f, 1.0f);
                        subsystem.draw_box(cx, cy, tile_width, tile_height);
                        subsystem.reset_color();
                        //break;
                    }
                } else {
                    /* draw void */
                    subsystem.set_color(0.5f, 0.0f, 0.0f, 1.0f);
                    subsystem.draw_box(cx, cy, tile_width, tile_height);
                    subsystem.reset_color();
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
}

void MapEditor::draw_objects() {
    if (wmap) {
        EditableMap::Objects& objects = wmap->get_objects();
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int tile_width = ts->get_tile_width();
            int tile_height = ts->get_tile_height();
            size_t sz = objects.size();
            for (size_t i = 0; i < sz; i++) {
                EditableObject *eobj = objects[i];
                int x = eobj->x * tile_width + left;
                int y = eobj->y * tile_height + top;
                subsystem.draw_tile(eobj->object->get_tile(), x, y);
            }
        }
    }
}

void MapEditor::draw_lights() {
if (wmap) {
        EditableMap::Lights& lights = wmap->get_light_sources();
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int tile_width = ts->get_tile_width();
            int tile_height = ts->get_tile_height();
            size_t sz = lights.size();
            for (size_t i = 0; i < sz; i++) {
                EditableLight *elgt = lights[i];
                int x = elgt->x * tile_width + left;
                int y = elgt->y * tile_height + top;
                subsystem.draw_icon(light_source, x, y);
            }
        }
    }
}

void MapEditor::place_tile(int x, int y, bool erasing) {
    x /= subsystem.get_zoom_factor();
    y /= subsystem.get_zoom_factor();
    x -= left;
    y -= top;

    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            x /= ts->get_tile_width();
            y /= ts->get_tile_height();
            if (x >= 0 && x < wmap->get_width() &&
                y >= 0 && y < wmap->get_height())
            {
                short tile_index = (erasing ? -1 : selected_tile_index);
                if (drawing_decoration) {
                    wmap->set_decoration(x, y, tile_index);
                } else {
                    wmap->set_tile(x, y, tile_index);
                }
                if (mirrored_drawing) {
                    int map_center = wmap->get_width() / 2;
                    int newx = map_center + (map_center - x) - 1;
                    if (drawing_decoration) {
                        wmap->set_decoration(newx, y, tile_index);
                    } else {
                        wmap->set_tile(newx, y, tile_index);
                    }
                }
            }
        }
    }
}

void MapEditor::place_object(int x, int y, bool erasing) {
    x /= subsystem.get_zoom_factor();
    y /= subsystem.get_zoom_factor();
    x -= left;
    y -= top;

    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            x /= ts->get_tile_width();
            y /= ts->get_tile_height();
            if (x >= 0 && x < wmap->get_width() &&
                y >= 0 && y < wmap->get_height())
            {
                Object *obj = static_cast<Object *>(resources.get_objects()[selected_object_index].object);
                if (erasing) {
                    wmap->erase_object(x, y);
                } else {
                    wmap->set_object(obj, x, y);
                }

                if (mirrored_drawing) {
                    int map_center = wmap->get_width() / 2;
                    int newx = map_center + (map_center - x) - 1;
                    if (erasing) {
                        wmap->erase_object(newx, y);
                    } else {
                        wmap->set_object(obj, newx, y);
                    }
                }
            }
        }
    }
}

void MapEditor::place_light(int x, int y, bool erasing) {
    x /= subsystem.get_zoom_factor();
    y /= subsystem.get_zoom_factor();
    x -= left;
    y -= top;

    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            x /= ts->get_tile_width();
            y /= ts->get_tile_height();
            if (x >= 0 && x < wmap->get_width() &&
                y >= 0 && y < wmap->get_height())
            {
                if (erasing) {
                    wmap->erase_light(x, y);
                } else {
                    wmap->set_light(x, y);
                }

                if (mirrored_drawing) {
                    int map_center = wmap->get_width() / 2;
                    int newx = map_center + (map_center - x) - 1;
                    if (erasing) {
                        wmap->erase_light(newx, y);
                    } else {
                        wmap->set_light(newx, y);
                    }
                }
            }
        }
    }
}

void MapEditor::mouse_move_map(int x, int y) {
    int zoom = subsystem.get_zoom_factor();
    x /= zoom;
    y /= zoom;
    left = move_origin_left + (x - move_origin_x);
    top = move_origin_top + (y - move_origin_y);
}

void MapEditor::edit_light(int x, int y) {
    x /= subsystem.get_zoom_factor();
    y /= subsystem.get_zoom_factor();
    x -= left;
    y -= top;

    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            x /= ts->get_tile_width();
            y /= ts->get_tile_height();
            if (x >= 0 && x < wmap->get_width() &&
                y >= 0 && y < wmap->get_height())
            {
                light_editor(wmap->get_light(x, y));
            }
        }
    }
}

void MapEditor::draw_rect(int x, int y, int w, int h) {
    subsystem.draw_box(x, y, 4, 1);
    subsystem.draw_box(x, y + 1, 1, 3);
    subsystem.draw_box(x + w - 4, y, 4, 1);
    subsystem.draw_box(x + w - 1, y + 1, 1, 3);
    subsystem.draw_box(x, y + h - 1, 4, 1);
    subsystem.draw_box(x, y + h - 4, 1, 3);
    subsystem.draw_box(x + w - 4, y + h - 1, 4, 1);
    subsystem.draw_box(x + w - 1, y + h - 4, 1, 3);
}

void MapEditor::copy_selection() {
    select_rect.clear();
    int x1 = select_rect.px[0];
    int y1 = select_rect.py[0];
    int x2 = select_rect.px[1];
    int y2 = select_rect.py[1];
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    select_rect.width = x2 - x1 + 1;
    select_rect.height = y2 - y1 + 1;
    select_rect.decoration = new short *[select_rect.height];
    select_rect.map = new short *[select_rect.height];
    for (int y = 0; y < select_rect.height; y++) {
        select_rect.decoration[y] = new short[select_rect.width];
        select_rect.map[y] = new short[select_rect.width];
        for (int x = 0; x < select_rect.width; x++) {
            select_rect.decoration[y][x] = wmap->get_decoration()[y + y1][x + x1];
            select_rect.map[y][x] = wmap->get_map()[y + y1][x + x1];

            EditableObject *object = wmap->get_object(x + x1, y + y1);
            if (object) {
                EditableObject new_object(*object);
                new_object.x = x;
                new_object.y = y;
                select_rect.objects.push_back(new_object);
            }

            EditableLight *light = wmap->get_light(x + x1, y + y1);
            if (light) {
                EditableLight new_light(*light);
                new_light.x = x;
                new_light.y = y;
                select_rect.lights.push_back(new_light);
            }
        }
    }
}

void MapEditor::draw_selection(int x, int y, bool decoration) {
    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int w = ts->get_tile_width();
            int h = ts->get_tile_height();
            int save_x = x;
            for (int ty = 0; ty < select_rect.height; ty++) {
                for (int tx = 0; tx < select_rect.width; tx++) {
                    int index = (decoration ? select_rect.decoration : select_rect.map)[ty][tx];
                    if (index > -1) {
                        Tile *tile = ts->get_tile(index);
                        subsystem.draw_tile(tile, x, y);
                    }
                    x += w;
                }
                x = save_x;
                y += h;
            }
        }
    }
}

void MapEditor::draw_selection_objects(int x, int y) {
    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int w = ts->get_tile_width();
            int h = ts->get_tile_height();
            for (SelectRect::Objects::iterator it = select_rect.objects.begin(); it != select_rect.objects.end(); it++) {
                subsystem.draw_tile(it->object->get_tile(), it->x * w + x, it->y * h + y);
            }
        }
    }
}

void MapEditor::draw_selection_lights(int x, int y) {
    if (wmap) {
        Tileset *ts = wmap->get_tileset_ptr();
        if (ts) {
            int w = ts->get_tile_width();
            int h = ts->get_tile_height();
            for (SelectRect::Lights::iterator it = select_rect.lights.begin(); it != select_rect.lights.end(); it++) {
                subsystem.draw_icon(light_source, it->x * w + x, it->y * h + y);
            }
        }
    }
}

void MapEditor::insert_selection(int x, int y, bool decoration) {
    if (wmap) {
        short **src = (decoration ? select_rect.decoration : select_rect.map);
        short **dst = (decoration ? wmap->get_decoration() : wmap->get_map());
        for (int sy = 0; sy < select_rect.height; sy++) {
            int dy = y + sy;
            if (dy < wmap->get_height()) {
                for (int sx = 0; sx < select_rect.width; sx++) {
                    int dx = x + sx;
                    if (dx < wmap->get_width()) {
                        if (dy >= 0 && dy < wmap->get_height() &&
                            dx >= 0 && dx < wmap->get_width() &&
                            src[sy][sx] > -1)
                        {
                            dst[dy][dx] = src[sy][sx];
                        }
                    }
                }
            }
        }
        wmap->touch();
    }
}

void MapEditor::insert_selection_objects(int x, int y) {
    if (wmap) {
        for (SelectRect::Objects::iterator it = select_rect.objects.begin(); it != select_rect.objects.end(); it++) {
            wmap->erase_object(it->x + x, it->y + y);
            wmap->set_object(it->object, it->x + x, it->y + y);
        }
    }
}

void MapEditor::insert_selection_lights(int x, int y) {
    if (wmap) {
        for (SelectRect::Lights::iterator it = select_rect.lights.begin(); it != select_rect.lights.end(); it++) {
            wmap->erase_light(it->x + x, it->y + y);
            EditableLight *light = wmap->set_light(it->x + x, it->y + y);
            light->radius = it->radius;
            light->r = it->r;
            light->g = it->g;
            light->b = it->b;
        }
    }
}

void MapEditor::static_center_map(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->center_map();
}
void MapEditor::center_map() {
    if (wmap->get_tileset_ptr()) {
        int mw = wmap->get_width();
        int mh = wmap->get_height();
        Tileset *ts = wmap->get_tileset_ptr();
        int tw = ts->get_tile_width();
        int th = ts->get_tile_height();
        left = -((mw * tw / 2) - subsystem.get_view_width() / 2);
        top = -((mh * th / 2) - subsystem.get_view_height() / 2);
    } else {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_NO_TILESET1), i18n(I18N_ME_NO_TILESET2));
    }
}

/* ************************************************************************** */
/* light editor                                                               */
/* ************************************************************************** */
void MapEditor::light_editor(EditableLight *light) {
    if (light) {
        lp_light = light;
        int width = get_subsystem().get_view_width();
        int height = get_subsystem().get_view_height();
        int ww = 210;
        int wh = 140;
        int tab = 80;
        char buffer[256];

        GuiWindow *window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_LIGHT_PROPERTIES));

        int y = Spc;
        int nl = 20;

        create_label(window, Spc, y + 1, i18n(I18N_ME_LP_RADIUS));
        sprintf(buffer, "%d", light->radius);
        lp_radius = create_textbox(window, tab, y, 50, buffer);
        create_label(window, tab + 50 + 10, y + 1, "(1 - 256)");
        y += nl;

        create_label(window, Spc, y + 1, i18n(I18N_ME_LP_RED));
        sprintf(buffer, "%d", light->r);
        lp_red = create_textbox(window, tab, y, 50, buffer);
        create_label(window, tab + 50 + 10, y + 1, "(0 - 255)");
        y += nl;

        create_label(window, Spc, y + 1, i18n(I18N_ME_LP_GREEN));
        sprintf(buffer, "%d", light->g);
        lp_green = create_textbox(window, tab, y, 50, buffer);
        create_label(window, tab + 50 + 10, y + 1, "(0 - 255)");
        y += nl;

        create_label(window, Spc, y + 1, i18n(I18N_ME_LP_BLUE));
        sprintf(buffer, "%d", light->b);
        lp_blue = create_textbox(window, tab, y, 50, buffer);
        create_label(window, tab + 50 + 10, y + 1, "(0 - 255)");
        y += nl;

        lp_radius->set_focus();
        add_ok_cancel_buttons(window, static_lp_ok_click);
    }
}

void MapEditor::static_lp_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->lp_ok_click();
}

void MapEditor::lp_ok_click() {
    int radius = atoi(lp_radius->get_text().c_str());
    if (radius < 1 || radius > 256) {
        lp_radius->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_RADIUS), i18n(I18N_ENTER_VALID_VALUE, "1-256"));
        return;
    }

    int r = atoi(lp_red->get_text().c_str());
    if (r < 0 || r > 255) {
        lp_red->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_RED), i18n(I18N_ENTER_VALID_VALUE, "1-255"));
        return;
    }

    int g = atoi(lp_green->get_text().c_str());
    if (g < 0 || g > 255) {
        lp_green->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_GREEN), i18n(I18N_ENTER_VALID_VALUE, "0-255"));
        return;
    }

    int b = atoi(lp_blue->get_text().c_str());
    if (b < 0 || b > 255) {
        lp_blue->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_BLUE), i18n(I18N_ENTER_VALID_VALUE, "0-255"));
        return;
    }

    lp_light->radius = radius;
    lp_light->r = r;
    lp_light->g = g;
    lp_light->b = b;
    wmap->touch();
    pop_window();
}

/* ************************************************************************** */
/* change mode                                                                */
/* ************************************************************************** */
void MapEditor::static_mode_selector_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->mode_selector_click();
}

void MapEditor::mode_selector_click() {
    int tmode = static_cast<int>(draw_mode);
    tmode++;
    if (tmode >= _DrawModeMAX) {
        tmode = 0;
    }
    draw_mode = static_cast<DrawMode>(tmode);
}

/* ************************************************************************** */
/* map properties                                                             */
/* ************************************************************************** */
void MapEditor::static_map_properties_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->map_properties_click();
}

void MapEditor::map_properties_click() {
    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();
    int ww = 460;
    int wh = 300;
    int tab = 90;
    char buffer[256];
    size_t sz;

    GuiWindow *window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_MP_TITLE));

    ww = window->get_client_width();
    wh = window->get_client_height();
    int lw = ww / 2 - 2 * Spc;
    int hx = ww / 2 + Spc;

    create_label(window, Spc, Spc + 1, i18n(I18N_ME_MP_NAME));
    mp_name = create_textbox(window, tab, Spc, lw - tab + Spc, wmap->get_name());

    create_label(window, hx, Spc + 1, i18n(I18N_ME_MP_FROG_SPAWN));
    sprintf(buffer, "%d", wmap->get_frog_spawn_init());
    mp_frog_spawn_init = create_textbox(window, hx + tab - Spc, Spc, 127, buffer);

    create_label(window, Spc, Spc + 21, i18n(I18N_ME_MP_AUTHOR));
    mp_author = create_textbox(window, tab, Spc + 20, ww - tab - Spc, wmap->get_author());

    create_label(window, Spc, Spc + 41, i18n(I18N_ME_MP_DESCRIPTION));
    mp_description = create_textbox(window, tab, Spc + 40, ww - tab - Spc, wmap->get_description());

    sprintf(buffer, "%d", wmap->get_width());
    create_label(window, Spc, Spc + 61, i18n(I18N_ME_MP_WIDTH));
    mp_width = create_textbox(window, tab, Spc + 60, 40, buffer);

    sprintf(buffer, "%d", wmap->get_height());
    create_label(window, Spc, Spc + 81, i18n(I18N_ME_MP_HEIGHT));
    mp_height = create_textbox(window, tab, Spc + 80, 40, buffer);

    sprintf(buffer, "%f", wmap->get_decoration_brightness());
    create_label(window, hx, Spc + 61, i18n(I18N_ME_MP_DECO_BRIGHTNESS));
    mp_deco_brightness = create_textbox(window, hx + tab - Spc, Spc + 60, 127, buffer);

    sprintf(buffer, "%f", wmap->get_lightmap_alpha());
    create_label(window, hx, Spc + 81, i18n(I18N_ME_MP_LIGHTMAP_ALPHA));
    mp_lightmap_alpha = create_textbox(window, hx + tab - Spc, Spc + 80, 127, buffer);

    sprintf(buffer, "%d", wmap->get_parallax_shift());
    create_label(window, hx, Spc + 101, i18n(I18N_ME_MP_PARALLAX));
    mp_parallax = create_textbox(window, hx + tab - Spc, Spc + 100, 40, buffer);

    create_label(window, Spc, Spc + 102, i18n(I18N_ME_MP_TYPE));
    mp_cb_dm = create_checkbox(window, tab, Spc + 103, "dm", false, static_mp_dm_click, this);
    mp_cb_tdm = create_checkbox(window, tab + 38, Spc + 103, "tdm", false, static_mp_tdm_click, this);
    mp_cb_ctf = create_checkbox(window, tab + 76, Spc + 103, "ctf", false, static_mp_ctf_click, this);

    mp_cb_sr = create_checkbox(window, tab, Spc + 116, "sr", false, static_mp_sr_click, this);
    mp_cb_ctc = create_checkbox(window, tab + 38, Spc + 116, "ctc", false, static_mp_ctc_click, this);
    mp_cb_goh = create_checkbox(window, tab + 76, Spc + 116, "goh", false, static_mp_goh_click, this);

    switch (wmap->get_game_play_type()) {
        case GamePlayTypeDM:
            mp_cb_dm->set_state(true);
            break;

        case GamePlayTypeTDM:
            mp_cb_tdm->set_state(true);
            break;

        case GamePlayTypeCTF:
            mp_cb_ctf->set_state(true);
            break;

        case GamePlayTypeSR:
            mp_cb_sr->set_state(true);
            break;

        case GamePlayTypeCTC:
            mp_cb_ctc->set_state(true);
            break;

        case GamePlayTypeGOH:
            mp_cb_goh->set_state(true);
            break;
    }

    /* create listboxes */
    create_label(window, Spc, Spc + 125, i18n(I18N_ME_MP_TILESET));
    mp_tileset = create_listbox(window, Spc, Spc + 140, lw, 93, "", 0, 0);
    Resources::ResourceObjects& tilesets = resources.get_tilesets();
    sz = tilesets.size();
    Tileset *mts = wmap->get_tileset_ptr();
    for (size_t i = 0; i < sz; i++) {
        Tileset *ts = static_cast<Tileset *>(tilesets[i].object);
        if (!ts->is_hidden_in_mapeditor()) {
            GuiListboxEntry *entry = mp_tileset->add_entry(ts->get_description());
            entry->set_ptr_tag(ts);
            if (ts == mts) {
                mp_tileset->set_selected_index(i);
                mp_tileset->set_top_index(i);
            }
        }
    }

    create_label(window, hx, Spc + 125, i18n(I18N_ME_MP_BACKGROUND));
    mp_background = create_listbox(window, hx, Spc + 140, lw, 93, "", 0, 0);
    Resources::ResourceObjects& backgrounds = resources.get_backgrounds();
    sz = backgrounds.size();
    Background *mbg = wmap->get_background_ptr();
    for (size_t i = 0; i < sz; i++) {
        Background *bg = static_cast<Background *>(backgrounds[i].object);
        GuiListboxEntry *entry = mp_background->add_entry(bg->get_description());
        entry->set_ptr_tag(bg);
        if (bg == mbg) {
            mp_background->set_selected_index(i);
            mp_background->set_top_index(i);
        }
    }

    add_ok_cancel_buttons(window, static_mp_ok_click);
    mp_name->set_focus();
}

void MapEditor::static_mp_dm_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_dm_click();
}

void MapEditor::mp_dm_click() {
    mp_cb_dm->set_state(true);
    mp_cb_tdm->set_state(false);
    mp_cb_ctf->set_state(false);
    mp_cb_sr->set_state(false);
    mp_cb_ctc->set_state(false);
    mp_cb_goh->set_state(false);
}

void MapEditor::static_mp_tdm_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_tdm_click();
}

void MapEditor::mp_tdm_click() {
    mp_cb_dm->set_state(false);
    mp_cb_tdm->set_state(true);
    mp_cb_ctf->set_state(false);
    mp_cb_sr->set_state(false);
    mp_cb_ctc->set_state(false);
    mp_cb_goh->set_state(false);
}

void MapEditor::static_mp_ctf_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_ctf_click();
}

void MapEditor::mp_ctf_click() {
    mp_cb_dm->set_state(false);
    mp_cb_tdm->set_state(false);
    mp_cb_ctf->set_state(true);
    mp_cb_sr->set_state(false);
    mp_cb_ctc->set_state(false);
    mp_cb_goh->set_state(false);
}

void MapEditor::static_mp_sr_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_sr_click();
}

void MapEditor::mp_sr_click() {
    mp_cb_dm->set_state(false);
    mp_cb_tdm->set_state(false);
    mp_cb_ctf->set_state(false);
    mp_cb_sr->set_state(true);
    mp_cb_ctc->set_state(false);
    mp_cb_goh->set_state(false);
}

void MapEditor::static_mp_ctc_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_ctc_click();
}

void MapEditor::mp_ctc_click() {
    mp_cb_dm->set_state(false);
    mp_cb_tdm->set_state(false);
    mp_cb_ctf->set_state(false);
    mp_cb_sr->set_state(false);
    mp_cb_ctc->set_state(true);
    mp_cb_goh->set_state(false);
}

void MapEditor::static_mp_goh_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->mp_goh_click();
}

void MapEditor::mp_goh_click() {
    mp_cb_dm->set_state(false);
    mp_cb_tdm->set_state(false);
    mp_cb_ctf->set_state(false);
    mp_cb_sr->set_state(false);
    mp_cb_ctc->set_state(false);
    mp_cb_goh->set_state(true);
}

void MapEditor::static_mp_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->mp_ok_click();
}

void MapEditor::mp_ok_click() {
    int len;

    /* validate */
    mp_name->set_text(trim(mp_name->get_text()));
    len = mp_name->get_text().length();
    if (!len || len > 20) {
        mp_name->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_NAME), i18n(I18N_ENTER_VALID_VALUE, "n<=20"));
        return;
    }

    int frog_spawn_init = atoi(mp_frog_spawn_init->get_text().c_str());
    if (frog_spawn_init < 0 || frog_spawn_init > 10000) {
        mp_frog_spawn_init->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_FROG_SPAWN), i18n(I18N_ENTER_VALID_VALUE, "0>=n<=10000"));
        return;
    }

    mp_author->set_text(trim(mp_author->get_text()));
    len = mp_author->get_text().length();
    if (!len || len > 60) {
        mp_author->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_AUTHOR), i18n(I18N_ENTER_VALID_VALUE, "n<=60"));
        return;
    }

    mp_description->set_text(trim(mp_description->get_text()));
    len = mp_description->get_text().length();
    if (!len || len > 60) {
        mp_description->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_DESCRIPTION), i18n(I18N_ENTER_VALID_VALUE, "n<=60"));
        return;
    }

    int width = atoi(mp_width->get_text().c_str());
    if (width < 40) {
        mp_width->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_WIDTH), i18n(I18N_ENTER_VALID_VALUE, "n>=40"));
        return;
    }

    if (width % 2) {
        mp_width->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_WIDTH), i18n(I18N_ME_WIDTH_EVEN));
        return;
    }

    int height = atoi(mp_height->get_text().c_str());
    if (height < 22) {
        mp_height->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_HEIGHT), i18n(I18N_ENTER_VALID_VALUE, "n>=22"));
        return;
    }

    double deco_brightness = atof(mp_deco_brightness->get_text().c_str());
    float lightmap_alpha = static_cast<float>(atof(mp_lightmap_alpha->get_text().c_str()));

    int parallax = atoi(mp_parallax->get_text().c_str());
    if (parallax < 1 || parallax > 1000) {
        mp_parallax->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_PARALLAX_SHIFT), i18n(I18N_ENTER_VALID_VALUE, "1<=n>=10000"));
        return;
    }

    int tileset_index = mp_tileset->get_selected_index();
    if (tileset_index < 0) {
        mp_tileset->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_TILESET), i18n(I18N_ME_TILESET_SELECT));
        return;
    }
    const Tileset *tileset = static_cast<const Tileset *>(mp_tileset->get_entry(tileset_index)->get_ptr_tag());

    int background_index = mp_background->get_selected_index();
    if (background_index < 0) {
        mp_background->set_focus();
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_BACKGROUND), i18n(I18N_ME_BACKGROUND_SELECT));
        return;
    }
    const Background *background = static_cast<const Background *>(mp_background->get_entry(background_index)->get_ptr_tag());

    /* ok */
    wmap->set_name(mp_name->get_text());
    wmap->set_author(mp_author->get_text());
    wmap->set_description(mp_description->get_text());
    wmap->set_decoration_brightness(deco_brightness);
    wmap->set_lightmap_alpha(lightmap_alpha);
    wmap->set_parallax_shift(parallax);
    wmap->resize_map(width, height);
    wmap->set_tileset(tileset->get_name());
    wmap->set_background(background->get_name());
    wmap->set_frog_spawn_init(frog_spawn_init);

    if (wmap->get_lightmap()) {
        wmap->get_lightmap()->set_alpha(lightmap_alpha);
    }

    if (mp_cb_dm->get_state()) {
        wmap->set_game_play_type(GamePlayTypeDM);
    } else if (mp_cb_tdm->get_state()) {
        wmap->set_game_play_type(GamePlayTypeTDM);
    } else if (mp_cb_ctf->get_state()) {
        wmap->set_game_play_type(GamePlayTypeCTF);
    } else if (mp_cb_sr->get_state()) {
        wmap->set_game_play_type(GamePlayTypeSR);
    } else if (mp_cb_ctc->get_state()) {
        wmap->set_game_play_type(GamePlayTypeCTC);
    } else if (mp_cb_goh->get_state()) {
        wmap->set_game_play_type(GamePlayTypeGOH);
    }

    tile_selector_page = 0;
    pop_window();
}

/* ************************************************************************** */
/* load map                                                                   */
/* ************************************************************************** */
void MapEditor::static_load_map_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->load_map_click();
}

void MapEditor::load_map_click() {
    if (wmap->is_modified()) {
        if (show_questionbox(i18n(I18N_ME_UNSAVED_MAP), i18n(I18N_ME_REALLY_CONTINUE)) == MessageBoxResponseNo) {
            return;
        }
    }

    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();
    int ww = 260;
    int wh = 180;

    GuiWindow *window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_LOAD_MAP_TITLE));
    ww = window->get_client_width();
    wh = window->get_client_height();

    lm_maps = create_listbox(window, Spc, Spc, ww - 2 * Spc, wh - 3 * Spc - 18, "", 0, 0);
    try {
        std::vector<std::string> entries;
        Directory dir(home_workdir + dir_separator + "maps", ".map", 0);
        const char *entry = 0;
        while ((entry = dir.get_entry())) {
            entries.push_back(entry);
        }
        std::sort(entries.begin(), entries.end());
        for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); it++) {
            lm_maps->add_entry(*it);
        }
        lm_maps->set_selected_index(0);
    } catch (const Exception&) {
        /* chomp */
    }

    add_ok_cancel_buttons(window, static_load_map_ok_click);
}

void MapEditor::static_load_map_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->load_map_ok_click();
}

void MapEditor::load_map_ok_click() {
    GuiListboxEntry *entry = lm_maps->get_entry(lm_maps->get_selected_index());
    if (entry) {
        try {
        std::string filename = home_workdir + dir_separator + "maps" + dir_separator;
        filename += entry->get_columns()[0].text;
        EditableMap *new_map = new EditableMap(resources, subsystem, filename);
        delete wmap;
        wmap = new_map;
        center_map();
        } catch (const Exception& e) {
            show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ME_ERROR_TITLE), e.what());
        }
        pop_window();
        tile_selector_page = 0;

    } else {
        show_messagebox(Gui::MessageBoxIconInformation, i18n(I18N_ME_NO_SELECTION), i18n(I18N_ME_SELECT_MAP));
        lm_maps->set_focus();
    }
}

/* ************************************************************************** */
/* save map                                                                   */
/* ************************************************************************** */
void MapEditor::static_save_map_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->save_map_click();
}

void MapEditor::save_map_click() {
    if (wmap) {
        if (!wmap->get_name().length()) {
            show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_SAVE_NO_MAP_NAME), i18n(I18N_ME_SAVE_CHOOSE_NAME));
        } else {
            try {
                create_directory("maps", home_workdir);
                wmap->save();
            } catch (const Exception& e) {
                show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
            }
        }
    }
}

/* ************************************************************************** */
/* tile selector                                                              */
/* ************************************************************************** */
void MapEditor::static_tile_selector_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->tile_selector_click();
}

void MapEditor::tile_selector_click() {
    Tileset *ts = wmap->get_tileset_ptr();
    if (!ts) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_NO_TILESET1), i18n(I18N_ME_NO_TILESET2));
        return;
    }

    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();

    int ww = width - 15;
    int wh = height - 100;

    ts_window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_SELECT_TILE));
    ts_window->set_on_keydown(static_ts_keydown, this);
    add_buttons(true, ts_window, 0);
    ts_buttons_on_page = ts_end_index + 1;

    /* jump to last opened page */
    int page = tile_selector_page;
    tile_selector_page = 0;
    for (int i = 0; i < page; i++) {
        ts_next_click();
    }
}

void MapEditor::add_buttons(bool first, GuiWindow *window, int from_tile_index) {
    window->remove_all_objects();
    ts_picture = 0;
    Tileset *ts = wmap->get_tileset_ptr();
    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();
    int tile_width = ts->get_tile_width();
    int tile_height = ts->get_tile_height();
    int ww = window->get_client_width();
    int wh = window->get_client_height();
    int x = Spc;
    int y = Spc;
    int wwcalc = ww;
    int whcalc = wh;
    size_t sz = ts->get_tile_count();

    ts_start_index = from_tile_index;
    ts_end_index = ts_start_index;
    int btncnt = 0;
    for (size_t i = from_tile_index; i < sz; i++) {
        GuiButton *btn = create_button(window, x, y, tile_width + 3, tile_height + 3, "", static_ts_tile_click, this);
        btn->set_tag(static_cast<int>(i));
        btn->show_bolts(false);
        GuiPicture *pic = create_picture(btn, 1, 1, ts->get_tile(i)->get_tilegraphic());
        if (static_cast<int>(i) == selected_tile_index) {
            ts_picture = pic;
        }
        x += tile_width + 2;
        if (x + tile_width + 2 > ww - 2 * Spc + (first ? 0 : tile_width + 2)) {
            wwcalc = x + 2 + Spc;
            x = Spc;
            y += tile_height + 2;
            if (y + tile_height + 2 > wh - 2 * Spc) {
                whcalc = y + 2 + Spc;
                break;
            }
        }
        btncnt++;
        if (!first && btncnt >= ts_buttons_on_page) {
            break;
        }
        ts_end_index++;
    }
    if (first) {
        window->set_width(wwcalc);
        whcalc += window->get_height() - window->get_client_height();
        window->set_height(whcalc + Spc + 18);
        window->set_x(width / 2 - wwcalc / 2);
        window->set_y(height / 2 - window->get_height() / 2);
    }
    add_close_button(window, static_ts_close_click);

    int bw = 39;
    int bh = 18;
    int bl = Spc;
    int bb = window->get_client_height() - bh - Spc;
    create_button(window, bl, bb, bw, bh, left_arrow, static_ts_previous_click, this);
    bl += bw + Spc;

    std::string btn_prop(i18n(I18N_ME_ST_PROPERTIES));
    int bw_prop = get_font()->get_text_width(btn_prop) + 28;
    create_button(window, bl, bb, bw_prop, bh, btn_prop, static_ts_properties_click, this);
    create_button(window, window->get_client_width() - bw - Spc, bb, bw, bh, right_arrow, static_ts_next_click, this);
}

void MapEditor::static_ts_previous_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_previous_click();
}

void MapEditor::ts_previous_click() {
    int new_index = ts_start_index - ts_buttons_on_page;
    if (new_index < 0) {
        new_index = 0;
    }
    add_buttons(false, ts_window, new_index);
    tile_selector_page--;
    if (tile_selector_page < 0) {
        tile_selector_page = 0;
    }
}

void MapEditor::static_ts_next_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_next_click();
}

void MapEditor::ts_next_click() {
    Tileset *ts = wmap->get_tileset_ptr();
    int sz = static_cast<int>(ts->get_tile_count());
    int new_index = ts_start_index + ts_buttons_on_page;
    if (new_index < sz) {
        add_buttons(false, ts_window, new_index);
        tile_selector_page++;
    }
}

void MapEditor::static_ts_tile_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_tile_click(sender);
}

void MapEditor::ts_tile_click(GuiVirtualButton *sender) {
    ts_picture = static_cast<GuiPicture *>(sender->get_children()[0]);
    selected_tile_index = sender->get_tag();
}

void MapEditor::static_ts_close_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_close_click();
}

bool MapEditor::static_ts_keydown(GuiWindow *sender, void *data, int keycode, bool repeat) {
    if (keycode == 27 || keycode == 't') {
        (reinterpret_cast<MapEditor *>(data))->ts_close_click();
        return true;
    }

    return false;
}

void MapEditor::ts_close_click() {
    ts_picture = 0;
    pop_window();
}

void MapEditor::static_ts_properties_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_properties_click();
}

void MapEditor::ts_properties_click() {
    if (selected_tile_index < 0) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_ST_NO_TILE1), i18n(I18N_ME_ST_NO_TILE2));
        return;
    }

    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();
    int ww = 300;
    int wh = 185 + Spc;
    char buffer[64];

    GuiWindow *window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_TP_TITLE, selected_tile_index));
    ww = window->get_client_width();
    wh = window->get_client_height();

    Tile *tile = wmap->get_tileset_ptr()->get_tile(selected_tile_index);
    tsp_background = create_checkbox(window, Spc, Spc, i18n(I18N_ME_TP_BACKGROUND), tile->is_background(), 0, 0);
    tsp_light_blocking = create_checkbox(window, Spc, Spc + 15, i18n(I18N_ME_TP_BLOCKS_LIGHT), tile->is_light_blocking(), 0, 0);

    create_label(window, Spc, 45, i18n(I18N_ME_TP_TILE_TYPE));
    int fh = get_font()->get_font_height();
    tsp_type = create_listbox(window, 110, 45, ww - 110 - Spc, fh * 4 + 2, "", 0, 0);
    int tiletype = static_cast<int>(tile->get_tile_type());
    int selected_tile = 0;
    for (int i = 0; i < Tile::_TileTypeMAX; i++) {
        tsp_type->add_entry(i18n(Tile::TypeDescription[i]));
        if (tiletype == i) {
            selected_tile = i;
        }
    }
    tsp_type->set_selected_index(selected_tile);
    tsp_type->set_top_index(selected_tile);

    create_label(window, Spc, 107, i18n(I18N_ME_TP_ANIMATION_SPEED));
    sprintf(buffer, "%d", tile->get_animation_speed());
    tsp_speed = create_textbox(window, 110, 106, 80, buffer);

    create_label(window, Spc, 127, i18n(I18N_ME_TP_FRICTION));
    sprintf(buffer, "%f", tile->get_friction());
    tsp_friction = create_textbox(window, 110, 126, 80, buffer);

    add_ok_cancel_buttons(window, static_ts_properties_ok_click);
}

void MapEditor::static_ts_properties_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->ts_properties_ok_click();
}

void MapEditor::ts_properties_ok_click() {
    try {
        Tile *tile = wmap->get_tileset_ptr()->get_tile(selected_tile_index);
        tile->set_is_background(tsp_background->get_state());
        tile->set_light_blocking(tsp_light_blocking->get_state());
        tile->set_type(static_cast<Tile::TileType>(tsp_type->get_selected_index()));
        tile->set_animation_speed(atoi(tsp_speed->get_text().c_str()));
        tile->set_friction(atof(tsp_friction->get_text().c_str()));
        save_tileset();
    } catch (const Exception& e) {
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
    }

    pop_window();
}

void MapEditor::save_tileset() throw (Exception) {
    char buffer[128];
    Tileset *ts = wmap->get_tileset_ptr();

    /* zap tileset kvp and rebuild it for saving */
    ts->clear();
    ts->set_value("name", ts->get_name());
    ts->set_value("author", ts->get_author());
    ts->set_value("description", ts->get_description());
    ts->set_value("width", ts->get_tile_width());
    ts->set_value("height", ts->get_tile_height());

    int tsz = static_cast<int>(ts->get_tile_count());
    for (int i = 0; i < tsz; i++) {
        Tile *tile = ts->get_tile(i);
        int sz = static_cast<int>(tile->get_tilegraphic()->get_tile_count());

        sprintf(buffer, "frames%d", i);
        ts->set_value(buffer, sz);

        sprintf(buffer, "animation_speed%d", i);
        ts->set_value(buffer, tile->get_animation_speed());

        sprintf(buffer, "tiletype%d", i);
        ts->set_value(buffer, static_cast<int>(tile->get_tile_type()));

        sprintf(buffer, "friction%d", i);
        ts->set_value(buffer, tile->get_friction());

        sprintf(buffer, "background%d", i);
        ts->set_value(buffer, tile->is_background());

        sprintf(buffer, "light_blocking%d", i);
        ts->set_value(buffer, tile->is_light_blocking());
    }

    ts->save(resources.get_resource_directory() + dir_separator + "tilesets" +
        dir_separator + ts->get_name() +".tileset");

    ts->untouch();
}

/* ************************************************************************** */
/* object selector                                                            */
/* ************************************************************************** */
void MapEditor::static_object_selector_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->object_selector_click();
}

void MapEditor::object_selector_click() {
    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();

    int ww = width - 260;
    int wh = height - 100;

    os_window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_SO_TITLE));
    os_window->set_on_keydown(static_os_keydown, this);

    ww = os_window->get_client_width();
    wh = os_window->get_client_height();

    Resources::ResourceObjects& objects = resources.get_objects();
    size_t sz = objects.size();
    int x = Spc;
    int y = Spc;
    int max_height = 0;
    for (size_t i = 0; i < sz; i++) {
        Object *obj = static_cast<Object *>(objects[i].object);
        if (!obj->is_spawnable()) {
            TileGraphic *tg = obj->get_tile()->get_tilegraphic();
            int obj_width = tg->get_width();
            int obj_height = tg->get_height();
            if (x + obj_width + 5 + Spc >= ww) {
                x = Spc;
                y += max_height + 5;
                max_height = 0;
            }
            GuiButton *btn = create_button(os_window, x, y, obj_width + 3, obj_height + 3, "", static_os_object_click, this);
            btn->set_tag(static_cast<int>(i));
            btn->show_bolts(false);
            GuiPicture *pic = create_picture(btn, 1, 1, tg);
            if (static_cast<int>(i) == selected_object_index) {
                os_picture = pic;
            }
            if (obj_height > max_height) {
                max_height = obj_height;
            }

            x += obj_width + 5;
        }
    }
    add_close_button(os_window, static_os_close_click);
}

void MapEditor::static_os_object_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->os_object_click(sender);
}

void MapEditor::os_object_click(GuiVirtualButton *sender) {
    os_picture = static_cast<GuiPicture *>(sender->get_children()[0]);
    selected_object_index = sender->get_tag();
}

bool MapEditor::static_os_keydown(GuiWindow *sender, void *data, int keycode, bool repeat) {
    if (keycode == 27 || keycode == 'o') {
        (reinterpret_cast<MapEditor *>(data))->os_close_click();
        return true;
    }

    return false;
}

void MapEditor::static_os_close_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->os_close_click();
}

void MapEditor::os_close_click() {
    os_picture = 0;
    pop_window();
}

/* ************************************************************************** */
/* horizontal copy                                                            */
/* ************************************************************************** */
void MapEditor::static_hcopy_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->hcopy_click();
}

void MapEditor::hcopy_click() {
    if (!wmap || !wmap->get_tileset_ptr()) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_HC_MAP_INVALID1), i18n(I18N_ME_HC_MAP_INVALID2));
        return;

    }
    if (show_questionbox(i18n(I18N_ME_HC_TITLE), i18n(I18N_ME_HC_QUESTION))
        == Gui::MessageBoxResponseNo)
    {
        return;
    }

    int map_width = wmap->get_width();
    int map_height = wmap->get_height();
    if (map_width % 2) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ME_HC_MAP_INVALID1), i18n(I18N_ME_HC_MAP_INVALID3));
        return;
    }

    int map_center = map_width / 2;
    short **map_array = wmap->get_map();
    for (int y = 0; y < map_height; y++) {
        for (int x = map_center - 1; x >= 0; x--) {
            int newx = map_center + (map_center - x) - 1;
            map_array[y][newx] = map_array[y][x];
        }
    }

    short **deco_array = wmap->get_decoration();
    for (int y = 0; y < map_height; y++) {
        for (int x = map_center - 1; x >= 0; x--) {
            int newx = map_center + (map_center - x) - 1;
            deco_array[y][newx] = deco_array[y][x];
        }
    }

    wmap->touch();
}

/* ************************************************************************** */
/* pack map                                                                   */
/* ************************************************************************** */
void MapEditor::static_pack_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->pack_click();
}

void MapEditor::pack_click() {
    if (!wmap || !wmap->is_valid_map()) {
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ME_PAK_NO_MAP), i18n(I18N_ME_PAK_CONFIGURE));
        return;
    }

    if (wmap->is_modified()) {
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ME_UNSAVED_MAP), i18n(I18N_ME_PAK_SAVE_FIRST));
        return;
    }

    std::string pak_name = home_workdir + dir_separator + wmap->get_name() + ".pak";
    if (Cargo::file_exists(pak_name)) {
        if (show_questionbox(i18n(I18N_ME_PAK_OVERWRITE1), i18n(I18N_ME_PAK_OVERWRITE2)) == MessageBoxResponseNo) {
            return;
        }
    }

    /* create package */
    try {
        SelectedFiles files;
        std::string name = "maps";
        name += dir_separator + wmap->get_name();
        files.push((name + ".map").c_str());
        if (Cargo::file_exists(home_workdir + dir_separator + name + ".lmp")) {
            files.push((name + ".lmp").c_str());
        }
        if (Cargo::file_exists(home_workdir + dir_separator + name + ".png")) {
            files.push((name + ".png").c_str());
        }
        Cargo cargo(home_workdir.c_str(), pak_name.c_str(), &files);
        cargo.pack();
        show_messagebox(Gui::MessageBoxIconInformation, i18n(I18N_ME_PAK_DONE1), i18n(I18N_ME_PAK_DONE2, wmap->get_name()));
    } catch (const Exception& e) {
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
    }
}

/* ************************************************************************** */
/* zap map                                                                    */
/* ************************************************************************** */
void MapEditor::static_zap_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->zap_click();
}

void MapEditor::zap_click() {
    if (wmap) {
        if (wmap->is_modified()) {
            if (show_questionbox(i18n(I18N_ME_UNSAVED_MAP), i18n(I18N_ME_REALLY_CONTINUE)) == MessageBoxResponseNo) {
                return;
            }
        }
        delete wmap;
    }

    wmap = new EditableMap(resources, subsystem);
    tile_selector_page = 0;
}

/* ************************************************************************** */
/* options                                                                    */
/* ************************************************************************** */
void MapEditor::static_options_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->options_click();
}

void MapEditor::options_click() {
    int width = get_subsystem().get_view_width();
    int height = get_subsystem().get_view_height();
    int ww = 249;
    int wh = 110;

    GuiWindow *window = push_window(width / 2 - ww / 2, height / 2 - wh / 2, ww, wh, i18n(I18N_ME_OPTIONS1));
    ww = window->get_client_width();
    wh = window->get_client_height();

    opt_old_fullscreen = subsystem.is_fullscreen();
    opt_old_scanlines = subsystem.has_scanlines();

    opt_fullscreen = create_checkbox(window, Spc, Spc, i18n(I18N_OPTIONS_SETTINGS21), opt_old_fullscreen, static_options_fs_clicked, this);
    opt_scanlines = create_checkbox(window, Spc, Spc + 15, i18n(I18N_OPTIONS_SETTINGS22), opt_old_scanlines, static_options_sl_clicked, this);

    create_label(window, Spc, Spc + 30, i18n(I18N_OPTIONS_SETTINGS23));
    sl_intensity_init_value = subsystem.get_scanlines_intensity();
    int v = static_cast<int>(sl_intensity_init_value * 100);
    opt_intensity = create_hscroll(window, Spc + 80, Spc + 31, 143, 25, 100, v, static_options_value_changed, this);

    add_ok_cancel_buttons(window, static_options_ok_click, static_options_cancel_click);
}

void MapEditor::static_options_value_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<MapEditor *>(data))->options_value_changed(value);
}

void MapEditor::options_value_changed(int value) {
    subsystem.set_scanlines_intensity(value / 100.0f);
}

void MapEditor::static_options_fs_clicked(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->options_fs_clicked(state);
}

void MapEditor::options_fs_clicked(bool state) {
    subsystem.toggle_fullscreen();
}

void MapEditor::static_options_sl_clicked(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MapEditor *>(data))->options_sl_clicked(state);
}

void MapEditor::options_sl_clicked(bool state) {
    subsystem.set_scanlines(state);
}

void MapEditor::static_options_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->options_ok_click();
}

void MapEditor::options_ok_click() {
    config.set_bool("fullscreen", opt_fullscreen->get_state());
    config.set_bool("show_scanlines", opt_scanlines->get_state());
    config.set_int("scanlines_intensity", opt_intensity->get_value());

    pop_window();
}

void MapEditor::static_options_cancel_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->options_cancel_click();
}

void MapEditor::options_cancel_click() {
    subsystem.set_scanlines_intensity(sl_intensity_init_value);
    if (opt_fullscreen->get_state() != opt_old_fullscreen) {
        subsystem.toggle_fullscreen();
    }
    subsystem.set_scanlines(opt_old_scanlines);
    pop_window();
}

/* ************************************************************************** */
/* exit editor                                                                */
/* ************************************************************************** */
void MapEditor::static_exit_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->exit_click();
}

void MapEditor::exit_click() {
    if (wmap->is_modified()) {
        if (show_questionbox(i18n(I18N_ME_UNSAVED_MAP), i18n(I18N_ME_REALLY_CONTINUE)) == Gui::MessageBoxResponseNo) {
            return;
        }
    }

    leave();
}
