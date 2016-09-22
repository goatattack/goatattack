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

#ifndef _OPTIONSMENU_HPP_
#define _OPTIONSMENU_HPP_

#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Gui.hpp"
#include "Configuration.hpp"
#include "OptionsMenu.hpp"
#include "KeyBinding.hpp"
#include "ButtonNavigator.hpp"

class Client;

class OptionsMenu {
private:
    OptionsMenu(const OptionsMenu&);
    OptionsMenu& operator=(const OptionsMenu&);

public:
    OptionsMenu(Gui& gui, Resources& resources, Subsystem& subsystem,
    Configuration& config, Client *client);
    virtual ~OptionsMenu();

    void refresh_options(bool force_game_over = false, int x = -1, int y = -1);
    void show_options(bool force_game_over = false, int x = -1, int y = -1);
    void close_options();
    bool are_options_visible() const;
    void get_options_window_position(int& x, int& y);

private:
    Gui& gui;
    Resources& resources;
    Subsystem& subsystem;
    Configuration& config;
    Client *client;
    bool options_visible;
    ButtonNavigator nav;
    GuiWindow *window;

    GuiTextbox *player_name;
    GuiCheckbox *show_player_name;
    GuiListbox *player_skin;
    GuiPicture *player_skin_pic;

    GuiTextbox *ck_up;
    GuiTextbox *ck_down;
    GuiTextbox *ck_left;
    GuiTextbox *ck_right;
    GuiTextbox *ck_jump;
    GuiTextbox *ck_fire;
    GuiTextbox *ck_drop1;
    GuiTextbox *ck_drop2;
    GuiTextbox *ck_drop3;
    GuiTextbox *ck_chat;
    GuiTextbox *ck_stats;
    GuiTextbox *ck_escape;
    GuiTextbox *ck_selected;
    GuiTextbox *ck_dz_h;
    GuiTextbox *ck_dz_v;

    GuiTextbox *gs_music_path;

    GuiTextbox *create_field(GuiWindow *parent, int x, int y,
        const std::string& text, GuiVirtualButton::OnClick on_click, bool erase_pic);
    void capture_key(GuiTextbox *ck_selected);

    static void static_nav_close(void *data);

    static void static_cancel_click(GuiObject *sender, void *data);
    static void static_close_options_click(GuiVirtualButton *sender, void *data);
    static void static_spectate_click(GuiVirtualButton *sender, void *data);
    static void static_skip_song_click(GuiVirtualButton *sender, void *data);
    static void static_back_options_click(GuiVirtualButton *sender, void *data);
    static void static_player_click(GuiVirtualButton *sender, void *data);
    static void static_player_skin_click(GuiListbox *sender, void *data, int index);
    static void static_close_player_click(GuiVirtualButton *sender, void *data);
    static void static_graphics_and_sound_click(GuiVirtualButton *sender, void *data);
    static void static_scanlines_intensity_changed(GuiVirtualScroll *sender, void *data, int value);
    static void static_music_volume_changed(GuiVirtualScroll *sender, void *data, int value);
    static void static_sfx_volume_changed(GuiVirtualScroll *sender, void *data, int value);
    static void static_text_fade_speed_changed(GuiVirtualScroll *sender, void *data, int value);
    static void static_controller_and_keyboard_click(GuiVirtualButton *sender, void *data);
    static void static_abort_capture_click(GuiVirtualButton *sender, void *data);
    static void static_capture_rescan_click(GuiVirtualButton *sender, void *data);
    static void static_capture_up_click(GuiVirtualButton *sender, void *data);
    static void static_capture_down_click(GuiVirtualButton *sender, void *data);
    static void static_capture_left_click(GuiVirtualButton *sender, void *data);
    static void static_capture_right_click(GuiVirtualButton *sender, void *data);
    static void static_capture_jump_click(GuiVirtualButton *sender, void *data);
    static void static_capture_fire_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop2_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop3_click(GuiVirtualButton *sender, void *data);
    static void static_capture_chat_click(GuiVirtualButton *sender, void *data);
    static void static_capture_stats_click(GuiVirtualButton *sender, void *data);
    static void static_capture_escape_click(GuiVirtualButton *sender, void *data);
    static bool static_capture_keydown(GuiWindow *sender, void *data, int keycode, bool repeat);
    static bool static_capture_joybuttondown(GuiWindow *sender, void *data, int button);

    static void static_ck_erase_horz(GuiVirtualButton *sender, void *data);
    static void static_ck_erase_vert(GuiVirtualButton *sender, void *data);

    static void static_close_window_click(GuiVirtualButton *sender, void *data);
    static void static_toggle_fullscreen_click(GuiCheckbox *sender, void *data, bool state);
    static void static_toggle_scanlines_click(GuiCheckbox *sender, void *data, bool state);

    static void static_close_capture_window_click(GuiVirtualButton *sender, void *data);

    void ck_erase_horz();
    void ck_erase_vert();

    void close_capture_window_click();
    void close_options_click();
    void spectate_click();
    void skip_song();
    void back_click();
    void close_window_click();
    void player_click();
    void player_skin_click(int index);
    void close_player_click();
    void graphics_and_sound_click();
    void scanlines_intensity_changed(int value);
    void music_volume_changed(int value);
    void sfx_volume_changed(int value);
    void text_fade_speed_changed(int value);
    void controller_and_keyboard_click();
    void abort_capture_click();
    void capture_rescan_click();
    void capture_up_click();
    void capture_down_click();
    void capture_left_click();
    void capture_right_click();
    void capture_jump_click();
    void capture_fire_click();
    void capture_drop1_click();
    void capture_drop2_click();
    void capture_drop3_click();
    void capture_chat_click();
    void capture_stats_click();
    void capture_escape_click();
    bool capture_keydown(int keycode);
    bool capture_joybuttondown(int button);
    void capture_draw();
    void capture_draw(MappedKey& mk, GuiTextbox *mktb);
    const char *capture_get_config_id();

    void toggle_fullscreen_click(bool state);
    void toggle_scanlines_click(bool state);

    virtual void options_closed() { }
};

#endif
