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

#include <cstdlib>

OptionsMenu::OptionsMenu(Gui& gui, Resources& resources, Subsystem& subsystem,
    Configuration& config, bool in_game)
    : gui(gui), resources(resources), subsystem(subsystem), config(config),
      in_game(in_game), options_visible(false), nav(gui, config) { }

OptionsMenu::~OptionsMenu() { }

void OptionsMenu::show_options() {
    if (!options_visible) {
        options_visible = true;
        int vw = subsystem.get_view_width();
        int vh = subsystem.get_view_height();
        int ww = 213;
        int wh = (in_game ? 235 : 165);
        int bw = 140;

        nav.clear();
        GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, "Options And Settings");
        if (!in_game) {
            window->set_cancelable(true);
            window->set_cancel_callback(static_cancel_click, this);
        }

        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 10, bw, 26, "Player", static_player_click, this));
        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 45, bw, 26, "Graphics And Sound", static_graphics_and_sound_click, this));
        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 80, bw, 26, "Controller And Keyboard", static_controller_and_keyboard_click, this));
        if (in_game) {
            nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 115, bw, 26, "Skip song", static_skip_song_click, this));
            nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 150, bw, 26, "Return To Main Menu", static_back_options_click, this));
        }

        nav.add_button(gui.create_button(window, ww / 2 - bw / 2, 185 - (in_game ? 0 : 70), bw, 26, "Close", static_close_options_click, this));

        if (in_game) {
            nav.install_handlers(window, static_nav_close, this);
            nav.set_button_focus();
        }
    }
}

bool OptionsMenu::are_options_visible() const {
    return options_visible;
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
    if (options_visible) {
        options_visible = false;
        gui.pop_window();
        options_closed();
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
    int wh = 167;
    int bw = 140;

    subsystem.clear_input_buffer();
    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, "Player");
    window->set_cancelable(true);

    gui.create_label(window, 15, 15, "player's name:");
    player_name = gui.create_textbox(window, 120, 15, 150, config.get_string("player_name"));
    show_player_name  = gui.create_checkbox(window, 15, 33, "show player's name", config.get_bool("show_player_name"), 0, 0);
    gui.create_label(window, 15, 53, "character:");
    player_skin = gui.create_listbox(window, 120, 56, 150, 60, "Character", static_player_skin_click, this);
    player_skin_pic = gui.create_picture(window, 15, 70, 0);

    Resources::ResourceObjects& sets = resources.get_charactersets();
    int sz = static_cast<int>(sets.size());
    for (int i = 0; i < sz; i++) {
        Characterset *cset = static_cast<Characterset *>(sets[i].object);
        player_skin->add_entry(cset->get_description());
        if (cset->get_name() == config.get_string("player_skin")) {
            player_skin->set_selected_index(i);
            player_skin_pic->set_picture(cset->get_tile(DirectionRight, CharacterAnimationStanding)->get_tilegraphic());
        }
    }

    player_name->set_focus();
    bw = 55;
    gui.create_button(window, ww / 2 - bw / 2, wh - 43, bw, 18, "Close", static_close_player_click, this);
}

void OptionsMenu::static_player_skin_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<OptionsMenu *>(data))->player_skin_click(index);
}

void OptionsMenu::player_skin_click(int index) {
    Characterset *cset = static_cast<Characterset *>(resources.get_charactersets()[index].object);
    player_skin_pic->set_picture(cset->get_tile(DirectionRight, CharacterAnimationStanding)->get_tilegraphic());
}

void OptionsMenu::static_close_player_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_player_click();
}

void OptionsMenu::close_player_click() {
    config.set_string("player_name", player_name->get_text());
    config.set_bool("show_player_name", show_player_name->get_state());
    config.set_string("player_skin", static_cast<Characterset *>(resources.get_charactersets()[player_skin->get_selected_index()].object)->get_name());
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
    int ww = 343;
    int wh = 243;
    int bw = 140;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, "Graphics And Sound");
    window->set_cancelable(true);

    gui.create_checkbox(window, 15, 15, "fullscreen graphics mode", subsystem.is_fullscreen(), static_toggle_fullscreen_click, this);
    gui.create_checkbox(window, 15, 30, "use fixed pipeline for drawing (change requires restart)", config.get_bool("fixed_pipeline"), static_toggle_render_mode_click, this);
    gui.create_checkbox(window, 15, 45, "draw scanlines", subsystem.has_scanlines(), static_toggle_scanlines_click, this);

    gui.create_label(window, 15, 60, "intensity:");
    gui.create_hscroll(window, 125, 61, 203, 25, 100, config.get_int("scanlines_intensity"), static_scanlines_intensity_changed, this);
    gui.create_box(window, 15, 81, ww - 30, 1);
    gui.create_label(window, 15, 90, "music volume:");
    gui.create_hscroll(window, 125, 91, 203, 0, 100, config.get_int("music_volume"), static_music_volume_changed, this);

    gui.create_label(window, 15, 105, "sfx volume:");
    gui.create_hscroll(window, 125, 106, 203, 0, 128, config.get_int("sfx_volume"), static_sfx_volume_changed, this);

    gui.create_box(window, 15, 126, ww - 30, 1);

    gui.create_label(window, 15, 135, "text fade speed:");
    gui.create_hscroll(window, 125, 136, 203, 5, 15, config.get_int("text_fade_speed"), static_text_fade_speed_changed, this);

    gui.create_box(window, 15, 156, ww - 30, 1);
    gui.create_label(window, 15, 165, "music player path:");
    gs_music_path = gui.create_textbox(window, 125, 165, 203, config.get_string("external_music"));
    gui.create_label(window, 125, 180, "(ogg and mp3, empty for internal)");

    bw = 55;
    gui.create_button(window, ww / 2 - bw / 2, wh - 43, bw, 18, "Close", static_close_sound_window_click, this);
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

void OptionsMenu::static_toggle_render_mode_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<OptionsMenu *>(data))->toggle_render_mode_click(state);
}

void OptionsMenu::toggle_render_mode_click(bool state) {
    config.set_bool("fixed_pipeline", state);
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

void OptionsMenu::static_close_sound_window_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_sound_window_click();
}

void OptionsMenu::close_sound_window_click() {
    std::string directory(gs_music_path->get_text());
    instant_trim(directory);
    if (config.get_string("external_music") != directory) {
        if (in_game) {
            gui.show_messagebox(Gui::MessageBoxIconInformation, "Music", "You need to rejoin for changes to take effect.");
        }
        config.set_string("external_music", directory);
    }
    gui.pop_window();
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
    int ww = 335;
    int wh = 203;
    int bw = 140;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, "Controller And Keyboard");
    window->set_cancelable(true);

    ck_up = create_field(window, 15, 15, "up:", static_capture_up_click, false);
    ck_down = create_field(window, 175, 15, "down:", static_capture_down_click, false);
    ck_left = create_field(window, 15, 35, "left:", static_capture_left_click, false);
    ck_right = create_field(window, 175, 35, "right:", static_capture_right_click, false);
    ck_jump = create_field(window, 15, 55, "jump:", static_capture_jump_click, false);
    ck_fire = create_field(window, 175, 55, "fire:", static_capture_fire_click, false);
    ck_drop1 = create_field(window, 15, 75, "grenade:", static_capture_drop1_click, false);
    ck_drop2 = create_field(window, 175, 75, "bomb:", static_capture_drop2_click, false);
    ck_drop3 = create_field(window, 15, 95, "frog:", static_capture_drop3_click, false);
    ck_chat = create_field(window, 175, 95, "chat:", static_capture_chat_click, false);
    ck_stats = create_field(window, 15, 115, "stats:", static_capture_stats_click, false);
    ck_escape = create_field(window, 175, 115, "esc:", static_capture_escape_click, false);
    ck_selected = 0;
    capture_draw();

    ck_dz_h = create_field(window, 15, 135, "dz horz.:", static_ck_erase_horz, true);
    ck_dz_h->set_text(config.get_string("deadzone_horizontal"));

    ck_dz_v = create_field(window, 175, 135, "dz vert.:", static_ck_erase_vert, true);
    ck_dz_v->set_text(config.get_string("deadzone_vertical"));

    bw = 55;
    gui.create_button(window, ww / 2 - bw / 2, wh - 43, bw, 18, "Close", static_close_capture_window_click, this);
    gui.create_button(window, Gui::Spc, wh - 43, 110, 18, "Rescan Joysticks", static_capture_rescan_click, this);
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
        gui.show_messagebox(Gui::MessageBoxIconExclamation, "Error", "Choose a value between 0 and 16000.");
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
    Icon *select = resources.get_icon("select");
    TileGraphic *stg = select->get_tile()->get_tilegraphic();
    int iw = stg->get_width();
    int ih = stg->get_height();

    gui.create_label(parent, x, y, text);
    GuiTextbox *tb = gui.create_textbox(parent, x + 50, y, 80, "");
    int tbh = tb->get_height();
    if (!erase_pic) {
        tb->set_locked(true);
        GuiButton *btn = gui.create_button(parent, x + 129, y, tbh, tbh, "", on_click, this);
        btn->show_bolts(false);
        gui.create_picture(btn, tbh / 2 - iw / 2, tbh / 2 - ih / 2, stg);
    } else {
        tb->set_locked(false);
        GuiButton *btn = gui.create_button(parent, x + 129, y, tbh, tbh, "...", on_click, this);
        btn->show_bolts(false);
    }

    return tb;
}

void OptionsMenu::capture_key(GuiTextbox *ck_selected) {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 200;
    int wh = 83;
    int bw = 140;

    this->ck_selected = ck_selected;

    GuiWindow *window = gui.push_window(vw / 2 - ww / 2, vh / 2- wh / 2, ww, wh, "Capturing");
    window->set_on_keydown(static_capture_keydown, this);
    window->set_on_joybuttondown(static_capture_joybuttondown, this);
    std::string text("Press key or use joystick...");
    Font *f = gui.get_font();
    int tw = f->get_text_width(text);
    int fh = f->get_font_height();
    gui.create_label(window, ww / 2 - tw / 2, window->get_client_height() / 2 - fh / 2 - 10, text);

    bw = 55;
    gui.create_button(window, ww / 2 - bw / 2, wh - 43, bw, 18, "Cancel", static_abort_capture_click, this);
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
    char buffer[128];
    sprintf(buffer, "%d joystick(s) found.", subsystem.rescan_joysticks());
    gui.show_messagebox(Gui::MessageBoxIconInformation, "Joysticks", buffer);
}

void OptionsMenu::static_capture_up_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_up_click();
}

void OptionsMenu::static_capture_down_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_down_click();
}

void OptionsMenu::static_capture_left_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_left_click();
}

void OptionsMenu::static_capture_right_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_right_click();
}

void OptionsMenu::static_capture_jump_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_jump_click();
}

void OptionsMenu::static_capture_fire_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_fire_click();
}

void OptionsMenu::static_capture_drop1_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop1_click();
}

void OptionsMenu::static_capture_drop2_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop2_click();
}

void OptionsMenu::static_capture_drop3_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_drop3_click();
}

void OptionsMenu::static_capture_chat_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_chat_click();
}

void OptionsMenu::static_capture_stats_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_stats_click();
}

void OptionsMenu::static_capture_escape_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->capture_escape_click();
}

void OptionsMenu::capture_up_click() {
    capture_key(ck_up);
}

void OptionsMenu::capture_down_click() {
    capture_key(ck_down);
}

void OptionsMenu::capture_left_click() {
    capture_key(ck_left);
}

void OptionsMenu::capture_right_click() {
    capture_key(ck_right);
}

void OptionsMenu::capture_jump_click() {
    capture_key(ck_jump);
}

void OptionsMenu::capture_fire_click() {
    capture_key(ck_fire);
}

void OptionsMenu::capture_drop1_click() {
    capture_key(ck_drop1);
}

void OptionsMenu::capture_drop2_click() {
    capture_key(ck_drop2);
}

void OptionsMenu::capture_drop3_click() {
    capture_key(ck_drop3);
}

void OptionsMenu::capture_chat_click() {
    capture_key(ck_chat);
}

void OptionsMenu::capture_stats_click() {
    capture_key(ck_stats);
}

void OptionsMenu::capture_escape_click() {
    capture_key(ck_escape);
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

const char *OptionsMenu::capture_get_config_id() {
    const char *key = 0;

    if (ck_selected == ck_up) {
        key = "up";
    } else if (ck_selected == ck_down) {
        key = "down";
    } else if (ck_selected == ck_left) {
        key = "left";
    } else if (ck_selected == ck_right) {
        key = "right";
    } else if (ck_selected == ck_jump) {
        key = "jump";
    } else if (ck_selected == ck_fire) {
        key = "fire";
    } else if (ck_selected == ck_drop1) {
        key = "drop1";
    } else if (ck_selected == ck_drop2) {
        key = "drop2";
    } else if (ck_selected == ck_drop3) {
        key = "drop3";
    } else if (ck_selected == ck_chat) {
        key = "chat";
    } else if (ck_selected == ck_stats) {
        key = "stats";
    } else if (ck_selected == ck_escape) {
        key = "escape";
    }

    return key;
}

void OptionsMenu::capture_draw() {
    KeyBinding binding;

    binding.extract_from_config(config);
    capture_draw(binding.left, ck_left);
    capture_draw(binding.right, ck_right);
    capture_draw(binding.up, ck_up);
    capture_draw(binding.down, ck_down);
    capture_draw(binding.jump, ck_jump);
    capture_draw(binding.fire, ck_fire);
    capture_draw(binding.drop1, ck_drop1);
    capture_draw(binding.drop2, ck_drop2);
    capture_draw(binding.drop3, ck_drop3);
    capture_draw(binding.chat, ck_chat);
    capture_draw(binding.stats, ck_stats);
    capture_draw(binding.escape, ck_escape);
}

void OptionsMenu::capture_draw(MappedKey& mk, GuiTextbox *mktb) {
    switch (mk.device) {
        case MappedKey::DeviceKeyboard:
        {
            const char *key = subsystem.get_keycode_name(mk.param);
            Font *f = gui.get_font();
            if (!f->get_text_width(key)) {
                mktb->set_text("???");
            } else {
                mktb->set_text(key);
            }
            break;
        }

        case MappedKey::DeviceJoyButton:
        {
            char buffer[16];
            sprintf(buffer, "%d", mk.param + 1);
            std::string joy;
            joy.assign(buffer);
            mktb->set_text("Button: "+ joy);
            break;
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
/* Close editor                                                               */
/* ************************************************************************** */
void OptionsMenu::static_close_window_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<OptionsMenu *>(data))->close_window_click();
}

void OptionsMenu::close_window_click() {
    gui.pop_window();
}
