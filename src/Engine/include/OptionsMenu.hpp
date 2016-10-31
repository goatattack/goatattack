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
#include "I18N.hpp"

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
    struct CharactersetEntry {
        CharactersetEntry(Characterset *cs) : cs(cs) { }

        Characterset *cs;

        bool operator<(const CharactersetEntry& rhs) const {
            return (cs->get_description() < rhs.cs->get_description());
        }
    };

    typedef std::vector<CharactersetEntry> CharactersetEntries;

    Gui& gui;
    Resources& resources;
    Subsystem& subsystem;

protected:
    I18N& i18n;

private:
    Configuration& config;
    Client *client;
    bool options_visible;
    ButtonNavigator nav;
    GuiWindow *window;

    GuiTextbox *player_name;
    GuiCheckbox *show_player_name;
    GuiListbox *player_skin;
    GuiPicture *player_skin_pic;
    GuiLabel *player_skin_name;

    GuiTextbox *ck_up[KeyBinding::MaxBindings];
    GuiTextbox *ck_down[KeyBinding::MaxBindings];
    GuiTextbox *ck_left[KeyBinding::MaxBindings];
    GuiTextbox *ck_right[KeyBinding::MaxBindings];
    GuiTextbox *ck_jump[KeyBinding::MaxBindings];
    GuiTextbox *ck_fire[KeyBinding::MaxBindings];
    GuiTextbox *ck_drop1[KeyBinding::MaxBindings];
    GuiTextbox *ck_drop2[KeyBinding::MaxBindings];
    GuiTextbox *ck_drop3[KeyBinding::MaxBindings];
    GuiTextbox *ck_chat[KeyBinding::MaxBindings];
    GuiTextbox *ck_stats[KeyBinding::MaxBindings];
    GuiTextbox *ck_escape[KeyBinding::MaxBindings];

    GuiTextbox *ck_selected;
    GuiTextbox *ck_dz_h;
    GuiTextbox *ck_dz_v;

    GuiTextbox *gs_music_path;

    GuiListbox *lang_lb;
    I18N::Language current_langugage;

    GuiTextbox *create_field(GuiWindow *parent, int x, int y,
        const std::string& text, GuiVirtualButton::OnClick on_click, bool erase_pic);

    void create_field2(GuiTextbox *dest[], GuiWindow *parent, int x, int y,
        const std::string& text, GuiVirtualButton::OnClick on_click_0,
        GuiVirtualButton::OnClick on_click_1, bool erase_pic);

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
    static void static_delete_capture_click(GuiVirtualButton *sender, void *data);
    static void static_capture_rescan_click(GuiVirtualButton *sender, void *data);

    static void static_capture_up_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_down_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_left_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_right_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_jump_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_fire_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop1_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop2_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop3_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_chat_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_stats_0_click(GuiVirtualButton *sender, void *data);
    static void static_capture_escape_0_click(GuiVirtualButton *sender, void *data);

    static void static_capture_up_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_down_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_left_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_right_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_jump_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_fire_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop1_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop2_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_drop3_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_chat_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_stats_1_click(GuiVirtualButton *sender, void *data);
    static void static_capture_escape_1_click(GuiVirtualButton *sender, void *data);

    static bool static_capture_keydown(GuiWindow *sender, void *data, int keycode, bool repeat);
    static bool static_capture_joybuttondown(GuiWindow *sender, void *data, int button);

    static void static_ck_erase_horz(GuiVirtualButton *sender, void *data);
    static void static_ck_erase_vert(GuiVirtualButton *sender, void *data);

    static void static_close_window_click(GuiVirtualButton *sender, void *data);
    static void static_toggle_fullscreen_click(GuiCheckbox *sender, void *data, bool state);
    static void static_toggle_scanlines_click(GuiCheckbox *sender, void *data, bool state);

    static void static_close_capture_window_click(GuiVirtualButton *sender, void *data);

    static void static_language_click(GuiVirtualButton *sender, void *data);
    static void static_language_ok_click(GuiVirtualButton *sender, void *data);

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
    void capture_up_click(int index);
    void capture_down_click(int index);
    void capture_left_click(int index);
    void capture_right_click(int index);
    void capture_jump_click(int index);
    void capture_fire_click(int index);
    void capture_drop1_click(int index);
    void capture_drop2_click(int index);
    void capture_drop3_click(int index);
    void capture_chat_click(int index);
    void capture_stats_click(int index);
    void capture_escape_click(int index);
    bool capture_keydown(int keycode);
    bool capture_joybuttondown(int button);
    void capture_draw();
    void capture_draw(MappedKey& mk, GuiTextbox *mktb);
    std::string capture_get_config_id();

    void toggle_fullscreen_click(bool state);
    void toggle_scanlines_click(bool state);

    void language_click();
    void language_ok_click();
    void add_ok_cancel_buttons(GuiWindow *window, GuiVirtualButton::OnClick on_click);

    static void static_change_button_texts(void *data);
    void change_button_texts();

    virtual void options_closed() { }
};

#endif
