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

#include "OptionsMenu.hpp"
#include "Client.hpp"

#include <algorithm>
#include <cstdlib>

OptionsMenu::OptionsMenu(Gui& gui, Resources& resources, Subsystem& subsystem,
    Configuration& config, Client *client)
    : gui(gui), resources(resources), subsystem(subsystem), i18n(subsystem.get_i18n()),
      config(config), client(client), options_visible(false), nav(gui, config), window(0) { }

OptionsMenu::~OptionsMenu() { }

void OptionsMenu::refresh_options(bool force_game_over, int x, int y) {
    if (options_visible) {
        options_visible = false;
        show_options(force_game_over, x, y);
    }
}

void OptionsMenu::show_options(bool force_game_over, int x, int y) {
    if (!options_visible) {
        options_visible = true;
        int vw = subsystem.get_view_width();
        int vh = subsystem.get_view_height();
        int ww = 233;
        int wh = 165;
        int bw = 160;
        int dh = 0;

        nav.clear();
        if (window && force_game_over) {
            window->remove_all_objects();
        }
        if (!force_game_over) {
            window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, i18n(I18N_MAINMENU_OPTIONS));
        }
        if (!client) {
            i18n.register_callback(static_change_button_texts, this);
            window->set_cancelable(true);
            window->set_cancel_callback(static_cancel_click, this);
        }

        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 10, bw, 26, i18n(I18N_OPTIONS_PLAYER), static_player_click, this))->set_tag(I18N_OPTIONS_PLAYER);
        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 45, bw, 26, i18n(I18N_OPTIONS_GRAPHICS_AND_SOUND), static_graphics_and_sound_click, this))->set_tag(I18N_OPTIONS_GRAPHICS_AND_SOUND);
        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 80, bw, 26, i18n(I18N_OPTIONS_CONTROLLER), static_controller_and_keyboard_click, this))->set_tag(I18N_OPTIONS_CONTROLLER);
        int top = 115;
        if (client) {
            if (!force_game_over && !client->is_game_over()) {
                if (!client->is_spectating()) {
                    nav.add_button(gui.create_button(window, ww / 2 - bw / 2, top + dh, bw, 26, i18n(I18N_OPTIONS_SPECTATE), static_spectate_click, this))->set_tag(I18N_OPTIONS_SPECTATE);
                    dh += 35;
                }
            }

            if (resources.get_musics().size()) {
                nav.add_button(gui.create_button(window, ww / 2 - bw / 2, top + dh, bw, 26, i18n(I18N_OPTIONS_SKIP_SONG), static_skip_song_click, this))->set_tag(I18N_OPTIONS_SKIP_SONG);
                dh += 35;
            }

            nav.add_button(gui.create_button(window, ww / 2 - bw / 2, top + dh, bw, 26, i18n(I18N_OPTIONS_RETURN), static_back_options_click, this))->set_tag(I18N_OPTIONS_RETURN);
            dh += 35;
        } else {
            nav.add_button(gui.create_button(window, ww / 2 - bw / 2, top + dh, bw, 26, i18n(I18N_LANGUAGE), static_language_click, this))->set_tag(I18N_LANGUAGE);
            dh += 35;
        }

        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, top + dh, bw, 26, i18n(I18N_BUTTON_CLOSE), static_close_options_click, this))->set_tag(I18N_BUTTON_CLOSE);

        if (client) {
            nav.install_handlers(window, static_nav_close, this);
            nav.set_button_focus();
        }

        /* readjust window */
        wh += dh;
        window->set_height(wh);
        if (x != -1 && y != -1) {
            window->set_x(x);
            window->set_y(y);
        } else {
            window->set_y(vh / 2- wh / 2);
        }
    }
}

void OptionsMenu::close_options() {
    if (options_visible) {
        options_visible = false;
        gui.pop_window();
        options_closed();
        if (!client) {
            i18n.unregister_callback(static_change_button_texts);
        }
        window = 0;
    }
}

bool OptionsMenu::are_options_visible() const {
    return options_visible;
}

void OptionsMenu::get_options_window_position(int& x, int& y) {
    if (options_visible) {
        x = window->get_x();
        y = window->get_y();
    } else {
        x = y = -1;
    }
}

void OptionsMenu::static_nav_close(void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_options_click();
}

void OptionsMenu::static_cancel_click(GuiObject *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_options_click();
}

void OptionsMenu::static_close_options_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_options_click();
}

void OptionsMenu::close_options_click() {
    close_options();
}

void OptionsMenu::static_spectate_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->spectate_click();
}

void OptionsMenu::spectate_click() {
    if (client) {
        client->spectate();
    }
}

void OptionsMenu::static_skip_song_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->skip_song();
}

void OptionsMenu::skip_song() {
    subsystem.skip_music_player_song();
}

void OptionsMenu::static_back_options_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->back_click();
}

void OptionsMenu::back_click() {
    gui.leave();
}

/* ************************************************************************** */
/* Player settings                                                            */
/* ************************************************************************** */
void OptionsMenu::static_player_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->player_click();
}

void OptionsMenu::player_click() {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 287;
    int wh = 193;

    subsystem.clear_input_buffer();
    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, i18n(I18N_OPTIONS_PLAYER));
    window->set_cancelable(true);

    gui.create_label(window, Gui::Spc, 16, i18n(I18N_OPTIONS_SETTINGS11));
    player_name = gui.create_textbox(window, 120, 15, 153, config.get_string("player_name"));
    show_player_name  = gui.create_checkbox(window, Gui::Spc, 35, i18n(I18N_OPTIONS_SETTINGS12), config.get_bool("show_player_name"), 0, 0);
    gui.create_label(window, Gui::Spc, 53, i18n(I18N_OPTIONS_SETTINGS13));
    player_skin = gui.create_listbox(window, 120, 56, 153, 80, "", static_player_skin_click, this);
    player_skin_pic = gui.create_picture(window, Gui::Spc, 70, 0);
    player_skin_name = gui.create_label(window, Gui::Spc, 70 + 32 + 3, "");

    /* read all charactersets and sort them */
    Resources::ResourceObjects& sets = resources.get_charactersets();
    int sz = static_cast<int>(sets.size());
    CharactersetEntries entries;
    for (int i = 0; i < sz; i++) {
        Characterset *cs = static_cast<Characterset *>(sets[i].object);
        entries.push_back(CharactersetEntry(cs));
    }
    std::sort(entries.begin(), entries.end());

    /* fill list */
    sz = static_cast<int>(entries.size());
    int selected_skin = 0;
    for (int i = 0; i < sz; i++) {
        Characterset *cs = entries[i].cs;
        player_skin->add_entry(cs->get_description())->set_ptr_tag(cs);
        if (cs->get_name() == config.get_string("player_skin")) {
            selected_skin = i;
        }
    }
    player_skin->set_selected_index(selected_skin);
    player_skin->set_top_index(selected_skin);
    player_name->set_focus();

    add_ok_cancel_buttons(window, static_close_player_click);
}

void OptionsMenu::static_player_skin_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<OptionsMenu *>(data))->player_skin_click(index);
}

void OptionsMenu::player_skin_click(int index) {
    const Characterset *cs = static_cast<const Characterset *>(player_skin->get_entry(index)->get_ptr_tag());
    player_skin_pic->set_picture(const_cast<Characterset *>(cs)->get_tile(DirectionRight, CharacterAnimationStanding)->get_tilegraphic());
    player_skin_name->set_caption(cs->get_name());
}

void OptionsMenu::static_close_player_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_player_click();
}

void OptionsMenu::close_player_click() {
    config.set_string("player_name", player_name->get_text());
    config.set_bool("show_player_name", show_player_name->get_state());
    const Characterset *cs = static_cast<const Characterset *>(player_skin->get_entry(player_skin->get_selected_index())->get_ptr_tag());
    config.set_string("player_skin", cs->get_name());
    gui.pop_window();
}

/* ************************************************************************** */
/* Graphics and sound settings                                                */
/* ************************************************************************** */
void OptionsMenu::static_graphics_and_sound_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->graphics_and_sound_click();
}

void OptionsMenu::graphics_and_sound_click() {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 285;
    int wh = 203;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, i18n(I18N_OPTIONS_GRAPHICS_AND_SOUND));
    window->set_cancelable(true);

    gui.create_checkbox(window, 15, 15, i18n(I18N_OPTIONS_SETTINGS21), subsystem.is_fullscreen(), static_toggle_fullscreen_click, this);
    gui.create_checkbox(window, 15, 30, i18n(I18N_OPTIONS_SETTINGS22), subsystem.has_scanlines(), static_toggle_scanlines_click, this);

    gui.create_label(window, 15, 45, i18n(I18N_OPTIONS_SETTINGS23));
    gui.create_hscroll(window, 130, 46, 140, 25, 100, config.get_int("scanlines_intensity"), static_scanlines_intensity_changed, this);
    gui.create_box(window, 15, 66, ww - 30, 1);

    gui.create_label(window, 15, 75, i18n(I18N_OPTIONS_SETTINGS24));
    gui.create_hscroll(window, 130, 76, 140, 0, 100, config.get_int("music_volume"), static_music_volume_changed, this);

    gui.create_label(window, 15, 90, i18n(I18N_OPTIONS_SETTINGS25));
    gui.create_hscroll(window, 130, 91, 140, 0, 128, config.get_int("sfx_volume"), static_sfx_volume_changed, this);
    gui.create_box(window, 15, 111, ww - 30, 1);

    gui.create_label(window, 15, 120, i18n(I18N_OPTIONS_SETTINGS26));
    gui.create_hscroll(window, 130, 121, 140, 5, 15, config.get_int("text_fade_speed"), static_text_fade_speed_changed, this);

    gui.create_checkbox(window, 15, 139, i18n(I18N_SHOW_LAGOMETER), config.get_bool("lagometer"), static_toggle_lagometer_click, this);

    std::string btn_close(i18n(I18N_BUTTON_CLOSE));
    int bw_close = gui.get_font()->get_text_width(btn_close) + 28;
    gui.create_button(window, ww / 2 - bw_close / 2, wh - 43, bw_close, 18, btn_close, static_close_window_click, this);
}

void OptionsMenu::static_toggle_fullscreen_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<OptionsMenu *>(data))->toggle_fullscreen_click(state);
}

void OptionsMenu::toggle_fullscreen_click(bool state) {
    subsystem.toggle_fullscreen();
    config.set_bool("fullscreen", state);
}

void OptionsMenu::static_toggle_scanlines_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<OptionsMenu *>(data))->toggle_scanlines_click(state);
}

void OptionsMenu::toggle_scanlines_click(bool state) {
    subsystem.set_scanlines(state);
    config.set_bool("show_scanlines", state);
}

void OptionsMenu::static_toggle_lagometer_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<OptionsMenu *>(data))->toggle_lagometer_click(state);
}

void OptionsMenu::toggle_lagometer_click(bool state) {
    config.set_bool("lagometer", state);
}

void OptionsMenu::static_scanlines_intensity_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<OptionsMenu *>(data))->scanlines_intensity_changed(value);
}

void OptionsMenu::scanlines_intensity_changed(int value) {
    float v = value / 100.0f;
    subsystem.set_scanlines_intensity(v);
    config.set_int("scanlines_intensity", value);
}

void OptionsMenu::static_music_volume_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<OptionsMenu *>(data))->music_volume_changed(value);
}

void OptionsMenu::music_volume_changed(int value) {
    subsystem.set_music_volume(value);
    config.set_int("music_volume", value);
}

void OptionsMenu::static_sfx_volume_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<OptionsMenu *>(data))->sfx_volume_changed(value);
}

void OptionsMenu::sfx_volume_changed(int value) {
    subsystem.set_sound_volume(value);
    config.set_int("sfx_volume", value);
}

void OptionsMenu::static_text_fade_speed_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<OptionsMenu *>(data))->text_fade_speed_changed(value);
}

void OptionsMenu::text_fade_speed_changed(int value) {
    config.set_int("text_fade_speed", value);
}

/* ************************************************************************** */
/* Controller and keyboard settings                                           */
/* ************************************************************************** */
void OptionsMenu::static_controller_and_keyboard_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->controller_and_keyboard_click();
}

void OptionsMenu::controller_and_keyboard_click() {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 560;
    int wh = 218;

    static const int Col0 = 15;
    static const int Col1 = 290;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_OPTIONS_CONTROLLER));
    window->set_cancelable(true);

    create_field2(ck_up, window, Col0, 15, i18n(I18N_OPTIONS_SETTINGS31), static_capture_up_0_click, static_capture_up_1_click, false);
    create_field2(ck_down, window, Col1, 15, i18n(I18N_OPTIONS_SETTINGS32), static_capture_down_0_click, static_capture_down_1_click, false);
    create_field2(ck_left, window, Col0, 35, i18n(I18N_OPTIONS_SETTINGS33), static_capture_left_0_click, static_capture_left_1_click, false);
    create_field2(ck_right, window, Col1, 35, i18n(I18N_OPTIONS_SETTINGS34), static_capture_right_0_click, static_capture_right_1_click, false);
    create_field2(ck_jump, window, Col0, 55, i18n(I18N_OPTIONS_SETTINGS35), static_capture_jump_0_click, static_capture_jump_1_click, false);
    create_field2(ck_fire, window, Col1, 55, i18n(I18N_OPTIONS_SETTINGS36), static_capture_fire_0_click, static_capture_fire_1_click, false);
    create_field2(ck_drop1, window, Col0, 75, i18n(I18N_OPTIONS_SETTINGS37), static_capture_drop1_0_click, static_capture_drop1_1_click, false);
    create_field2(ck_drop2, window, Col1, 75, i18n(I18N_OPTIONS_SETTINGS38), static_capture_drop2_0_click, static_capture_drop2_1_click, false);
    create_field2(ck_drop3, window, Col0, 95, i18n(I18N_OPTIONS_SETTINGS39), static_capture_drop3_0_click, static_capture_drop3_1_click, false);
    create_field2(ck_chat, window, Col1, 95, i18n(I18N_OPTIONS_SETTINGS40), static_capture_chat_0_click, static_capture_chat_1_click, false);
    create_field2(ck_stats, window, Col0, 115, i18n(I18N_OPTIONS_SETTINGS41), static_capture_stats_0_click, static_capture_stats_1_click, false);
    create_field2(ck_escape, window, Col1, 115, i18n(I18N_OPTIONS_SETTINGS42), static_capture_escape_0_click, static_capture_escape_1_click, false);

    ck_selected = 0;
    capture_draw();

    gui.create_box(window, 15, 139, ww - 15 * 2 - 2, 1);

    ck_dz_h = create_field(window, Col0, 150, i18n(I18N_OPTIONS_SETTINGS43), static_ck_erase_horz, true);
    ck_dz_h->set_text(config.get_string("deadzone_horizontal"));
    ck_dz_h->set_type(GuiTextbox::TypeInteger);

    ck_dz_v = create_field(window, Col1, 150, i18n(I18N_OPTIONS_SETTINGS44), static_ck_erase_vert, true);
    ck_dz_v->set_text(config.get_string("deadzone_vertical"));
    ck_dz_v->set_type(GuiTextbox::TypeInteger);

    std::string btn_close(i18n(I18N_BUTTON_CLOSE));
    int bw_close = gui.get_font()->get_text_width(btn_close) + 28;
    gui.create_button(window, ww / 2 - bw_close / 2, wh - 43, bw_close, 18, btn_close, static_close_capture_window_click, this);

    gui.create_button(window, Gui::Spc, wh - 43, 110, 18, i18n(I18N_OPTIONS_SETTINGS45), static_capture_rescan_click, this);
}

void OptionsMenu::static_ck_erase_horz(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->ck_erase_horz();
}

void OptionsMenu::ck_erase_horz() {
    ck_dz_h->set_text("3200");
    ck_dz_h->set_focus();
}

void OptionsMenu::static_ck_erase_vert(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->ck_erase_vert();
}

void OptionsMenu::ck_erase_vert() {
    ck_dz_v->set_text("3200");
    ck_dz_v->set_focus();
}

void OptionsMenu::static_close_capture_window_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_capture_window_click();
}

void OptionsMenu::close_capture_window_click() {
    int dzh = atoi(ck_dz_h->get_text().c_str());
    int dzv = atoi(ck_dz_v->get_text().c_str());
    GuiTextbox *focus_tb = 0;

    if (dzh < 0 || dzh > 16000) {
        focus_tb = ck_dz_h;
    } else if (dzv < 0 || dzv > 16000) {
        focus_tb = ck_dz_v;
    }

    if (focus_tb) {
        gui.show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_ERROR_TITLE), i18n(I18N_MAINMENU_INVALID_GENERIC_NUMBER, "0 - 16000"));
        focus_tb->set_focus();
        return;
    }
    config.set_int("deadzone_horizontal", dzh);
    config.set_int("deadzone_vertical", dzv);
    subsystem.set_deadzone_horizontal(dzh);
    subsystem.set_deadzone_vertical(dzv);

    close_window_click();
}

GuiTextbox *OptionsMenu::create_field(GuiWindow *parent, int x, int y,
    const std::string& text, GuiVirtualButton::OnClick on_click, bool erase_pic)
{

    gui.create_label(parent, x, y + 1, text);
    GuiTextbox *tb = gui.create_textbox(parent, x + 160, y, 80, "");
    int tbh = tb->get_height();
    if (!erase_pic) {
        tb->set_locked(true);
        GuiButton *btn = gui.create_button(parent, x + 239, y, tbh, tbh, "", on_click, this);
        btn->show_bolts(false);
        Icon *select = resources.get_icon("select");
        TileGraphic *stg = select->get_tile()->get_tilegraphic();
        int iw = stg->get_width();
        int ih = stg->get_height();
        gui.create_picture(btn, tbh / 2 - iw / 2, tbh / 2 - ih / 2, stg);
    } else {
        tb->set_locked(false);
        GuiButton *btn = gui.create_button(parent, x + 239, y, tbh, tbh, "", on_click, this);
        btn->show_bolts(false);
        Icon *select = resources.get_icon("default");
        TileGraphic *stg = select->get_tile()->get_tilegraphic();
        int iw = stg->get_width();
        int ih = stg->get_height();
        gui.create_picture(btn, tbh / 2 - iw / 2, tbh / 2 - ih / 2, stg);
    }

    return tb;
}

void OptionsMenu::create_field2(GuiTextbox *dest[], GuiWindow *parent, int x, int y,
    const std::string& text, GuiVirtualButton::OnClick on_click_0,
    GuiVirtualButton::OnClick on_click_1, bool erase_pic)
{
    static const int TextboxWidth = 80;
    static const int TextboxSpacer = 20;

    Icon *select = resources.get_icon("select");
    TileGraphic *stg = select->get_tile()->get_tilegraphic();
    int iw = stg->get_width();
    int ih = stg->get_height();

    gui.create_label(parent, x, y + 1, text);
    for (int i = 0; i < 2; i++) {
        GuiVirtualButton::OnClick on_click = (i == 0 ? on_click_0 : on_click_1);
        if (on_click) {
            int xoffs = (i * (TextboxWidth + TextboxSpacer));
            GuiTextbox *tb = gui.create_textbox(parent, x + 60 + xoffs, y, TextboxWidth, "");
            int tbh = tb->get_height();
            if (!erase_pic) {
                tb->set_locked(true);
                GuiButton *btn = gui.create_button(parent, x + TextboxWidth + 59 + xoffs, y, tbh, tbh, "", on_click, this);
                btn->show_bolts(false);
                gui.create_picture(btn, tbh / 2 - iw / 2, tbh / 2 - ih / 2, stg);
            } else {
                tb->set_locked(false);
                GuiButton *btn = gui.create_button(parent, x + TextboxWidth + 59 + xoffs, y, tbh, tbh, "...", on_click, this);
                btn->show_bolts(false);
            }
            dest[i] = tb;
        }
    }
}

void OptionsMenu::capture_key(GuiTextbox *ck_selected) {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 320;
    int wh = 83;

    this->ck_selected = ck_selected;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_OPTIONS_SETTINGS46));
    window->set_on_keydown(static_capture_keydown, this);
    window->set_on_joybuttondown(static_capture_joybuttondown, this);
    std::string text(i18n(I18N_OPTIONS_SETTINGS47));

    Font *f = gui.get_font();

    int tw = f->get_text_width(text);
    int fh = f->get_font_height();
    gui.create_label(window, ww / 2 - tw / 2, window->get_client_height() / 2 - fh / 2 - 10, text);

    std::string btn_delete(i18n(I18N_BUTTON_DELETE));
    int bw_delete = f->get_text_width(btn_delete) + 28;

    std::string btn_cancel(i18n(I18N_BUTTON_CANCEL));
    int bw_cancel = f->get_text_width(btn_cancel) + 28;

    gui.create_button(window, Gui::Spc, wh - 43, bw_delete, 18, btn_delete, static_delete_capture_click, this);
    gui.create_button(window, ww - bw_cancel - Gui::Spc, wh - 43, bw_cancel, 18, btn_cancel, static_abort_capture_click, this);
}

void OptionsMenu::static_delete_capture_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_keydown(0);
}

void OptionsMenu::static_abort_capture_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->abort_capture_click();
}

void OptionsMenu::abort_capture_click() {
    ck_selected = 0;
    gui.pop_window();
}

void OptionsMenu::static_capture_rescan_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_rescan_click();
}

void OptionsMenu::capture_rescan_click() {
    gui.show_messagebox(Gui::MessageBoxIconInformation, i18n(I18N_OPTIONS_SETTINGS49), i18n(I18N_OPTIONS_SETTINGS48, subsystem.rescan_joysticks()));
}

void OptionsMenu::static_capture_up_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_up_click(0);
}

void OptionsMenu::static_capture_down_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_down_click(0);
}

void OptionsMenu::static_capture_left_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_left_click(0);
}

void OptionsMenu::static_capture_right_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_right_click(0);
}

void OptionsMenu::static_capture_jump_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_jump_click(0);
}

void OptionsMenu::static_capture_fire_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_fire_click(0);
}

void OptionsMenu::static_capture_drop1_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop1_click(0);
}

void OptionsMenu::static_capture_drop2_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop2_click(0);
}

void OptionsMenu::static_capture_drop3_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop3_click(0);
}

void OptionsMenu::static_capture_chat_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_chat_click(0);
}

void OptionsMenu::static_capture_stats_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_stats_click(0);
}

void OptionsMenu::static_capture_escape_0_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_escape_click(0);
}

void OptionsMenu::static_capture_up_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_up_click(1);
}

void OptionsMenu::static_capture_down_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_down_click(1);
}

void OptionsMenu::static_capture_left_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_left_click(1);
}

void OptionsMenu::static_capture_right_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_right_click(1);
}

void OptionsMenu::static_capture_jump_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_jump_click(1);
}

void OptionsMenu::static_capture_fire_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_fire_click(1);
}

void OptionsMenu::static_capture_drop1_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop1_click(1);
}

void OptionsMenu::static_capture_drop2_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop2_click(1);
}

void OptionsMenu::static_capture_drop3_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop3_click(1);
}

void OptionsMenu::static_capture_chat_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_chat_click(1);
}

void OptionsMenu::static_capture_stats_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_stats_click(1);
}

void OptionsMenu::static_capture_escape_1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_escape_click(1);
}

void OptionsMenu::capture_up_click(int index) {
    capture_key(ck_up[index]);
}

void OptionsMenu::capture_down_click(int index) {
    capture_key(ck_down[index]);
}

void OptionsMenu::capture_left_click(int index) {
    capture_key(ck_left[index]);
}

void OptionsMenu::capture_right_click(int index) {
    capture_key(ck_right[index]);
}

void OptionsMenu::capture_jump_click(int index) {
    capture_key(ck_jump[index]);
}

void OptionsMenu::capture_fire_click(int index) {
    capture_key(ck_fire[index]);
}

void OptionsMenu::capture_drop1_click(int index) {
    capture_key(ck_drop1[index]);
}

void OptionsMenu::capture_drop2_click(int index) {
    capture_key(ck_drop2[index]);
}

void OptionsMenu::capture_drop3_click(int index) {
    capture_key(ck_drop3[index]);
}

void OptionsMenu::capture_chat_click(int index) {
    capture_key(ck_chat[index]);
}

void OptionsMenu::capture_stats_click(int index) {
    capture_key(ck_stats[index]);
}

void OptionsMenu::capture_escape_click(int index) {
    capture_key(ck_escape[index]);
}

bool OptionsMenu::static_capture_keydown(GuiWindow *sender, void *data, int keycode, bool repeat) {
    return (reinterpret_cast<OptionsMenu *>(data))->capture_keydown(keycode);
}

bool OptionsMenu::capture_keydown(int keycode) {
    gui.pop_window();

    std::string key(capture_get_config_id());
    MappedKey binding(MappedKey::DeviceKeyboard, keycode);
    KeyBinding::write_binding(config, binding, key);
    capture_draw(binding, ck_selected);
    ck_selected = 0;

    return true;
}

std::string OptionsMenu::capture_get_config_id() {
    const char *key = 0;

    for (int i = 0; i < KeyBinding::MaxBindings; i++) {
        if (ck_selected == ck_up[i]) {
            key = "up";
        } else if (ck_selected == ck_down[i]) {
            key = "down";
        } else if (ck_selected == ck_left[i]) {
            key = "left";
        } else if (ck_selected == ck_right[i]) {
            key = "right";
        } else if (ck_selected == ck_jump[i]) {
            key = "jump";
        } else if (ck_selected == ck_fire[i]) {
            key = "fire";
        } else if (ck_selected == ck_drop1[i]) {
            key = "drop1";
        } else if (ck_selected == ck_drop2[i]) {
            key = "drop2";
        } else if (ck_selected == ck_drop3[i]) {
            key = "drop3";
        } else if (ck_selected == ck_chat[i]) {
            key = "chat";
        } else if (ck_selected == ck_stats[i]) {
            key = "stats";
        } else if (ck_selected == ck_escape[i]) {
            key = "escape";
        }
        if (key) {
            std::string new_key(key);
            new_key += KeyBinding::get_suffix(i);
            return new_key;
        }
    }

    return "";
}

void OptionsMenu::capture_draw() {
    KeyBinding binding;
    binding.extract_from_config(config);
    for (int i = 0; i < KeyBinding::MaxBindings; i++) {
        capture_draw(binding.left[i], ck_left[i]);
        capture_draw(binding.right[i], ck_right[i]);
        capture_draw(binding.up[i], ck_up[i]);
        capture_draw(binding.down[i], ck_down[i]);
        capture_draw(binding.jump[i], ck_jump[i]);
        capture_draw(binding.fire[i], ck_fire[i]);
        capture_draw(binding.drop1[i], ck_drop1[i]);
        capture_draw(binding.drop2[i], ck_drop2[i]);
        capture_draw(binding.drop3[i], ck_drop3[i]);
        capture_draw(binding.chat[i], ck_chat[i]);
        capture_draw(binding.stats[i], ck_stats[i]);
        capture_draw(binding.escape[i], ck_escape[i]);
    }
}

void OptionsMenu::capture_draw(MappedKey& mk, GuiTextbox *mktb) {
    if (mktb) {
        switch (mk.device) {
            case MappedKey::DeviceKeyboard:
            {
                const char *key = subsystem.get_keycode_name(mk.param);
                Font *f = gui.get_font();
                if (!f->get_text_width(key)) {
                    mktb->set_text(i18n(I18N_OPTIONS_SETTINGS50));
                } else {
                    mktb->set_text(key);
                }
                break;
            }

            case MappedKey::DeviceJoyButton:
            {
                mktb->set_text(i18n(I18N_OPTIONS_SETTINGS51, mk.param + 1));
                break;
            }
        }
    }
}

bool OptionsMenu::static_capture_joybuttondown(GuiWindow *sender, void *data, int button) {
    return (reinterpret_cast<OptionsMenu *>(data))->capture_joybuttondown(button);
}

bool OptionsMenu::capture_joybuttondown(int button) {
    gui.pop_window();
    std::string key(capture_get_config_id());
    MappedKey binding(MappedKey::DeviceJoyButton, button);
    KeyBinding::write_binding(config, binding, key);
    capture_draw(binding, ck_selected);
    ck_selected = 0;
    return true;
}

/* ************************************************************************** */
/* Language                                                                   */
/* ************************************************************************** */
void OptionsMenu::static_language_click(GuiVirtualButton *sender, void *data) {
    reinterpret_cast<OptionsMenu *>(data)->language_click();
}

void OptionsMenu::language_click() {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 220;
    int wh = 160;

    int lng = config.get_int("language");
    current_langugage = static_cast<I18N::Language>(lng);

    subsystem.clear_input_buffer();
    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_LANGUAGE));
    window->set_cancelable(true);

    lang_lb = gui.create_listbox(window, Gui::Spc, Gui::Spc, ww - 2 * Gui::Spc - 2, wh - 4 * Gui::Spc - 20, "", 0, 0);

    const char **lang = I18N::Languages;

    int selected_lang = 0;
    int cur = 0;
    while (*lang) {
        lang_lb->add_entry(*lang);
        if (lng == cur) {
            selected_lang = cur;
        }
        lang++;
        cur++;
    }
    lang_lb->set_selected_index(selected_lang);

    add_ok_cancel_buttons(window, static_language_ok_click);
}

void OptionsMenu::static_language_ok_click(GuiVirtualButton *sender, void *data) {
    reinterpret_cast<OptionsMenu* >(data)->language_ok_click();
}

void OptionsMenu::language_ok_click() {
    I18N::Language new_language = static_cast<I18N::Language>(lang_lb->get_selected_index());
    if (new_language != current_langugage) {
        config.set_int("language", static_cast<int>(new_language));
        i18n.change(new_language);
    }
    gui.pop_window();
}

void OptionsMenu::add_ok_cancel_buttons(GuiWindow *window, GuiVirtualButton::OnClick on_click) {
    std::string btn_cancel(i18n(I18N_BUTTON_CANCEL));
    int bw_cancel = gui.get_font()->get_text_width(btn_cancel) + 24;

    std::string btn_ok(i18n(I18N_BUTTON_OK));
    int bw_ok = gui.get_font()->get_text_width(btn_ok) + 24;

    int width = window->get_client_width();
    int height = window->get_client_height();
    int bh = 18;
    gui.create_button(window, width - bw_cancel - Gui::Spc, height - bh - Gui::Spc, bw_cancel, bh, btn_cancel, static_close_window_click, this);
    gui.create_button(window, width - bw_cancel - Gui::Spc - bw_ok - 5, height - bh - Gui::Spc, bw_ok, bh, btn_ok, on_click, this);
}

/* ************************************************************************** */
/* Close window                                                               */
/* ************************************************************************** */
void OptionsMenu::static_close_window_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_window_click();
}

void OptionsMenu::close_window_click() {
    gui.pop_window();
}

/* ************************************************************************** */
/* Change language callback                                                   */
/* ************************************************************************** */
void OptionsMenu::static_change_button_texts(void *data) {
    reinterpret_cast<OptionsMenu *>(data)->change_button_texts();
}

void OptionsMenu::change_button_texts() {
    const ButtonNavigator::Buttons& btns = nav.get_buttons();
    for (ButtonNavigator::Buttons::const_iterator it = btns.begin(); it != btns.end(); it++) {
        GuiButton *btn = *it;
        btn->set_caption(i18n(static_cast<I18NText>(btn->get_tag())));
    }
}

