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

#ifndef _MAINMENU_HPP_
#define _MAINMENU_HPP_

#include "Exception.hpp"
#include "GuiObject.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Gui.hpp"
#include "Configuration.hpp"
#include "OptionsMenu.hpp"
#include "LANBroadcaster.hpp"
#include "MasterQuery.hpp"

#include <vector>

class MainMenu : public Gui, protected OptionsMenu {
private:
    MainMenu(const MainMenu&);
    MainMenu& operator=(const MainMenu&);

public:
    MainMenu(Resources& resources, Subsystem& subsystem, Configuration& config);
    ~MainMenu();

    virtual void idle();
    virtual void on_input_event(const InputData& input);

protected:
    virtual void on_leave();

private:
    enum MenuButtonState {
        MenuButtonStateNone = 0,
        MenuButtonStateCreating,
        MenuButtonStateFinalize,
        MenuButtonStateDone
    };

    struct MapEntry {
        MapEntry(Map *map, int index) : map(map), index(index) { }

        Map *map;
        int index;
    };

    typedef std::vector<MapEntry> MapEntries;

    struct ListSort {
        ListSort() : sort(Ascending), index(0) { }

        ServerSort sort;
        int index;
    };

    Resources& resources;
    Subsystem& subsystem;
    I18N& i18n;
    Configuration& config;
    int x;
    int y;
    int bgox;
    int bgoy;
    Icon *goat;
    Icon *title;
    int gw;
    int gh;
    Icon *brick;
    int bw;
    int bh;
    MenuButtonState shown;
    LANBroadcaster *lan_broadcaster;
    MasterQuery *master_query;
    GuiWindow *main_window;
    int mw_w;
    int mw_h;
    Sound *menu_construction;
    Music *title_music;
    GuiLabel *version_label;
    GuiPicture *beta;

    gametime_t now;
    gametime_t startup;
    gametime_t last;
    gametime_t last_lan_info;

    GuiTextbox *cs_server_name;
    GuiTextbox *cs_server_port;
    GuiTextbox *cs_server_password;
    GuiCheckbox *cs_server_public;
    GuiCheckbox *cb_server_show_pwd;
    GuiCheckbox *cs_dm;
    GuiCheckbox *cs_tdm;
    GuiCheckbox *cs_ctf;
    GuiCheckbox *cs_sr;
    GuiCheckbox *cs_ctc;
    GuiCheckbox *cs_goh;
    GuiCheckbox *cs_current_mode;
    GuiTextbox *cs_max_players;
    GuiTextbox *cs_warmup;
    GuiTextbox *cs_duration;
    GuiLabel *cs_map_name;
    GuiPicture *cs_map_preview;
    GuiListbox *cs_maps;
    GuiListbox *play_lan_list;
    GuiListbox *play_wan_list;
    GuiCheckbox *play_lan_compatible;
    GuiCheckbox *play_wan_compatible;

    GuiTextbox *custom_ipaddress;
    GuiTextbox *custom_port;
    GuiTextbox *custom_password;
    GuiRoundedButton *rb[6];
    GuiTab *creation_tab;
    GuiCheckbox *cs_opt_friendly_fire;
    GuiCheckbox *cs_opt_shooting_explosives;
    GuiCheckbox *cs_opt_prevent_pick;
    GuiCheckbox *cs_opt_refuse_join;
    GuiCheckbox *cs_opt_hold_disconnected_players;
    GuiTextbox *cs_opt_reconnect_kills;

    const void *wan_list_selected_entry;
    const void *lan_list_selected_entry;

    void drawmenu(Font *font, const char **entries);

    /* statics for button callbacks */
    static void static_play_click(GuiVirtualButton *sender, void *data);
    static void static_cancel_click(GuiObject *sender, void *data);
    static void static_play_close(GuiVirtualButton *sender, void *data);
    void play_close();
    static void static_play_manual(GuiVirtualButton *sender, void *data);
    void play_manual();
    static void static_play_connect_lan_click(GuiVirtualButton *sender, void *data);
    void play_connect_lan_click();
    static void static_play_refresh_lan_click(GuiVirtualButton *sender, void *data);
    void play_refresh_lan_click();

    static void static_play_connect_wan_click(GuiVirtualButton *sender, void *data);
    void play_connect_wan_click();
    static void static_play_refresh_wan_click(GuiVirtualButton *sender, void *data);
    void play_refresh_wan_click();

    static void static_show_pwd_click(GuiCheckbox *sender, void *data, bool state);
    void show_pwd_click(bool state);

    static void static_create_server_click(GuiVirtualButton *sender, void *data);
    static void static_game_mode_click(GuiCheckbox *sender, void *data, bool state);
    static void static_map_selected(GuiListbox *sender, void *data, int index);
    static void static_start_server_click(GuiVirtualButton *sender, void *data);
    static void static_close_start_server_click(GuiVirtualButton *sender, void *data);

    static void static_list_packages_click(GuiVirtualButton *sender, void *data);
    void list_packages_click();
    static void static_close_packages_list_click(GuiVirtualButton *sender, void *data);
    void close_packages_list_click();

    static void static_options_click(GuiVirtualButton *sender, void *data);
    static void static_credits_click(GuiVirtualButton *sender, void *data);
    static void static_quit_click(GuiVirtualButton *sender, void *data);

    static void static_close_window_click(GuiVirtualButton *sender, void *data);

    static void static_play_wan_sort_click(GuiListbox *sender, void *data, int index);
    static void static_play_lan_sort_click(GuiListbox *sender, void *data, int index);
    void play_wan_sort_click(int index);
    void play_lan_sort_click(int index);

    static void static_on_wan_entry_click(GuiListbox *sender, void *data, int index);
    void on_wan_entry_click(int index);

    static void static_on_lan_entry_click(GuiListbox *sender, void *data, int index);
    void on_lan_entry_click(int index);

    static bool static_on_tab_click(GuiTab *sender, int index, void *data);

    static void on_compatible_click(GuiCheckbox *sender, void *data, bool state);
    void compatible_click(bool state);

    /* other functions */
    void play_click();

    void fill_map_listbox(GamePlayType game_play_type);
    static bool MapEntryCompare(const MapEntry& lhs, const MapEntry& rhs);
    void create_server_click();
    void game_mode_click(GuiCheckbox *sender);
    void map_selected(int index);
    void server_validate(bool close);

    bool map_is_valid(GamePlayType selected_type, GamePlayType map_type);

    void options_click();
    void credits_click();

    void close_window_click();

    void create_server_locator(int index);
    void destroy_server_locators();

    static void static_lang_change_cb(void *data);
    void change_button_captions();
    void set_version_label();
};

#endif
