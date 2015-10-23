#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Exception.hpp"
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
    virtual ~MainMenu();

    virtual void idle() throw (Exception);
    virtual void on_input_event(const InputData& input);

private:
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
    bool shown;
    LANBroadcaster *lan_broadcaster;
    MasterQuery *master_query;

    gametime_t now;
    gametime_t startup;
    gametime_t last;
    gametime_t last_lan_info;

    GuiTextbox *cs_server_name;
    GuiTextbox *cs_server_port;
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

    const void *wan_list_selected_entry;
    const void *lan_list_selected_entry;

    void drawmenu(Font *font, const char **entries);

    /* statics for button callbacks */
    static void static_play_click(GuiVirtualButton *sender, void *data);
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

    static void static_create_server_click(GuiVirtualButton *sender, void *data);
    static void static_game_mode_click(GuiCheckbox *sender, void *data, bool state);
    static void static_map_selected(GuiListbox *sender, void *data, int index);
    static void static_start_server_click(GuiVirtualButton *sender, void *data);
    static void static_close_start_server_click(GuiVirtualButton *sender, void *data);

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

    bool create_server_locators() throw (Exception);
    void destroy_server_locators();
};

#endif
