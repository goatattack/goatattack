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

#include "MainMenu.hpp"
#include "Timing.hpp"
#include "Globals.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "TournamentFactory.hpp"
#include "OptionsMenu.hpp"
#include "Protocol.hpp"
#include "Scope.hpp"
#include "UTF8.hpp"

#include <cstdlib>
#include <algorithm>

namespace {

    const char *TitleMusic = "norway";
    const int FlagWidth = 18;
    const char *AscendingArrow = "\xe2\x86\x91";
    const char *DescendingArrow = "\xe2\x86\x93";

}

class BroadcasterGuard {
private:
    BroadcasterGuard(const BroadcasterGuard&);
    BroadcasterGuard& operator=(const BroadcasterGuard&);

public:
    BroadcasterGuard(LANBroadcaster *lb, MasterQuery *mq) : lb(lb), mq(mq) {
        if (lb) lb->stop();
        if (mq) mq->stop();
    }

    ~BroadcasterGuard() {
        if (lb) lb->start();
        if (mq) mq->start();
    }

public:
    LANBroadcaster *lb;
    MasterQuery *mq;
};

MainMenu::MainMenu(Resources& resources, Subsystem& subsystem, Configuration& config)
    : Gui(resources, subsystem, resources.get_font("normal")),
      OptionsMenu(*this, resources, subsystem, config, 0),
      resources(resources), subsystem(subsystem), i18n(subsystem.get_i18n()),
      config(config), x(0), y(0), bgox(0), bgoy(0), goat(0), title(0), gw(0),
      gh(0), shown(MenuButtonStateNone), lan_broadcaster(0), master_query(0),
      main_window(0), mw_w(0), mw_h(0),
      menu_construction(resources.get_sound("menu_construction")),
      title_music(0), version_label(0), beta(0)
{
    i18n.register_callback(this->static_lang_change_cb, this);

    goat = resources.get_icon("title_goat");
    title = resources.get_icon("title_text");

    gw = goat->get_tile()->get_tilegraphic()->get_width();
    gh = goat->get_tile()->get_tilegraphic()->get_height();

    brick = resources.get_icon("title_brick");
    bw = brick->get_tile()->get_tilegraphic()->get_width();
    bh = brick->get_tile()->get_tilegraphic()->get_height();

    get_now(startup);
    last = startup;

    /* looking for title music */
    try {
        title_music = resources.get_music(TitleMusic);
    } catch (...) {
        /* chomp */
    }
    subsystem.play_music(title_music);

    memset(rb, 0, sizeof rb);
}

MainMenu::~MainMenu() {
    if (lan_broadcaster) {
        delete lan_broadcaster;
    }
}

void MainMenu::idle() {
    static char buffer[64];
    get_now(now);

    /* first delay, then show main menu */
    if (shown != MenuButtonStateDone) {
        ms_t diff = diff_ms(startup, now);
        switch (shown) {
            case MenuButtonStateNone:
            {
                if (diff > 1300) {
                    startup = now;
                    shown = MenuButtonStateCreating;

                    int vw = subsystem.get_view_width();
                    int vh = subsystem.get_view_height();
                    mw_w = 600;
                    mw_h = 300;
                    main_window = push_window(vw / 2 - mw_w / 2, vh / 2 - mw_h / 2, mw_w, mw_h, "");
                    main_window->set_invisible(true);
                    set_mouse_xy(subsystem.get_view_width() / 2, subsystem.get_view_height() / 2);
                }
                break;
            }

            case MenuButtonStateCreating:
            {
                if (diff > 50) {
                    startup = now;

                    int bw = 160;
                    int bh = 35;
                    const int yofs = 30;

                    if (!rb[0]) {
                        rb[0] = create_rounded_button(main_window, 20, 30 + yofs, bw, bh, i18n(I18N_MAINMENU_PLAY), static_play_click, this);
                        rb[0]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                    } else if (!rb[1]) {
                        rb[1] = create_rounded_button(main_window, 17, 90 + yofs, bw, bh, i18n(I18N_MAINMENU_LAN), static_create_server_click, this);
                        rb[1]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                    } else if (!rb[2]) {
                        rb[2] = create_rounded_button(main_window, 20, 150 + yofs, bw, bh, i18n(I18N_MAINMENU_PACKAGES), static_list_packages_click, this);
                        rb[2]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                    } else if (!rb[3]) {
                        rb[3] = create_rounded_button(main_window, 420, 30 + yofs, bw, bh, i18n(I18N_MAINMENU_OPTIONS), static_options_click, this);
                        rb[3]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                    } else if (!rb[4]) {
                        rb[4] = create_rounded_button(main_window, 423, 90 + yofs, bw, bh, i18n(I18N_MAINMENU_CREDITS), static_credits_click, this);
                        rb[4]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                    } else if (!rb[5]) {
                        rb[5] = create_rounded_button(main_window, 420, 150 + yofs, bw, bh, i18n(I18N_MAINMENU_QUIT), static_quit_click, this);
                        rb[5]->set_follow_alpha(false);
                        subsystem.play_sound(menu_construction, 1);
                        shown = MenuButtonStateFinalize;
                    }
                }
                break;
            }

            case MenuButtonStateFinalize:
            {
                if (diff > 100) {
                    startup = now;
                    shown = MenuButtonStateDone;
                    set_version_label();
                }
                break;
            }

            case MenuButtonStateDone:
                break;
        }
    }

    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();

    /* retrieving LAN server infos */
    if (lan_broadcaster) {
        /* refresh */
        ms_t diff = diff_ms(last_lan_info, now);
        if (diff > 2000) {
            try {
                Scope<Mutex> lock(lan_broadcaster->get_mutex());
                lan_broadcaster->refresh();
            } catch (const Exception&) {
                /* chomp */
            }
            last_lan_info = now;
        }
        /* compatible servers only? */
        bool compatible = config.get_bool("compatible_servers_only");
        /* update list */
        int top_index = play_lan_list->get_top_index();
        const void *selected_ptr = lan_list_selected_entry;
        lan_list_selected_entry = 0;
        play_lan_list->clear();
        Scope<Mutex> lock(lan_broadcaster->get_mutex());
        const Hosts& hosts = lan_broadcaster->get_hosts();
        for (Hosts::const_iterator it = hosts.begin();
            it != hosts.end(); it++)
        {
            const GameserverInformation *info = *it;
            if (!compatible || info->protocol_version == ProtocolVersion) {
                std::string server_name(info->server_name);
                Icon *flag = resources.get_flag_from_name(server_name);
                GuiListboxEntry *entry = play_lan_list->add_entry(flag, FlagWidth, server_name);
                sprintf(buffer, "%d", info->protocol_version);
                entry->add_column(buffer, 40);
                entry->add_column((info->secured ? resources.get_icon("lock") : 0), 9, "", 16);
                sprintf(buffer, "%d/%d", info->cur_clients, info->max_clients);
                entry->add_column(buffer, 50);
                sprintf(buffer, "%d", static_cast<int>(info->ping_time));
                entry->add_column(buffer, 40);
                entry->set_ptr_tag(info);
                if (selected_ptr == info) {
                    lan_list_selected_entry = info;
                    play_lan_list->set_selected_index(play_lan_list->get_entry_count() - 1);
                }
            }
        }
        play_lan_list->set_top_index(top_index);
    }

    /* retrieving WAN server infos */
    if (master_query) {
        /* compatible servers only? */
        bool compatible = config.get_bool("compatible_servers_only");
        /* update list */
        int top_index = play_wan_list->get_top_index();
        play_wan_list->clear();
        Scope<Mutex> lock(master_query->get_mutex());
        const Hosts& hosts = master_query->get_hosts();
        for (Hosts::const_iterator it = hosts.begin();
            it != hosts.end(); it++)
        {
            const MasterQueryClient *info = static_cast<MasterQueryClient *>(*it);
            if (info->received) {
                if (!compatible || info->protocol_version == ProtocolVersion) {
                    std::string server_name(info->server_name);
                    Icon *flag = resources.get_flag_from_name(server_name);
                    GuiListboxEntry *entry = play_wan_list->add_entry(flag, FlagWidth, server_name);
                    sprintf(buffer, "%d", info->protocol_version);
                    entry->add_column(buffer, 40);
                    entry->add_column((info->secured ? resources.get_icon("lock") : 0), 9, "", 16);
                    sprintf(buffer, "%d/%d", info->cur_clients, info->max_clients);
                    entry->add_column(buffer, 50);
                    sprintf(buffer, "%d", static_cast<int>(info->ping_time));
                    entry->add_column(buffer, 40);
                    entry->set_ptr_tag(info);
                    if (info == wan_list_selected_entry) {
                        play_wan_list->set_selected_index(play_wan_list->get_entry_count() - 1);
                    }
                }
            }
        }
        play_wan_list->set_top_index(top_index);
    }

    /* draw background */
    ms_t diff = diff_ms(last, now);
    if (diff > 25) {
        last = now;
        bgox--;
        bgoy--;
        bgox %= bw;
        bgoy %= bh;
    }
    y = bgoy;
    subsystem.set_color(1.0f, 1.0f, 1.0f, 0.35f);
    while (y < vh) {
        x = bgox;
        while (x < vw) {
            subsystem.draw_icon(brick, x, y);
            x += bw;
        }
        y += bh;
    }
    subsystem.reset_color();

    /* draw goat */
    subsystem.set_color(1.0f, 1.0f, 1.0f, 1);
    x = vw / 2 - gw / 2;
    y = vh / 2 - gh / 2;
    subsystem.draw_icon(title, x, y);
    subsystem.draw_icon(goat, x, y);
    subsystem.reset_color();
}

void MainMenu::on_input_event(const InputData& input) { }

void MainMenu::on_leave() { }

/* ************************************************************************** */
/* Play                                                                       */
/* ************************************************************************** */
void MainMenu::static_play_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_click();
}

void MainMenu::play_click() {
    Font *f = get_font();

    std::string btn_close(i18n(I18N_BUTTON_CLOSE));
    int bw_close = f->get_text_width(btn_close) + 28;

    std::string btn_connect(i18n(I18N_MAINMENU_CONNECT));
    int bw_connect = f->get_text_width(btn_connect) + 28;

    std::string btn_refresh(i18n(I18N_MAINMENU_REFRESH));
    int bw_refresh = f->get_text_width(btn_refresh) + 28;

    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 460;
    int wh = 300;

    GuiWindow *window = push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_MAINMENU_PLAY));
    window->set_cancelable(true);
    window->set_cancel_callback(static_cancel_click, this);

    int wcw = window->get_client_width();
    int wch = window->get_client_height();
    int bh = 18;

    create_button(window, wcw / 2 - bw_close / 2, window->get_client_height() - bh - Spc, bw_close, bh, btn_close, static_play_close, this);

    GuiTab *tab = create_tab(window, Spc, Spc, wcw - (2 * Spc), wch - bh - (3 * Spc));
    tab->set_on_tab_click(static_on_tab_click, this);
    create_server_locator(0);

    GuiListboxEntry *title_bar;
	bool compatible = config.get_bool("compatible_servers_only");

    /* LAN */
    GuiFrame *frlan = tab->create_tab(i18n(I18N_MAINMENU_LAN_TITLE));
	play_lan_compatible = create_checkbox(frlan, 0, 0, i18n(I18N_MAINMENU_COMPATIBLE_SERVERS), compatible, on_compatible_click, this);
    create_label(frlan, 0, 15, i18n(I18N_MAINMENU_LAN_SERVERS));
    play_lan_list = create_listbox(frlan, 0, 30, frlan->get_width(), frlan->get_height() + 2 - (30 + bh + Spc), 0, FlagWidth, i18n(I18N_MAINMENU_SERVER_NAME), static_on_lan_entry_click, this);
    play_lan_list->show_title_bar(true);
    play_lan_list->set_on_title_clicked(static_play_lan_sort_click, this);
    create_button(frlan, frlan->get_width() - bw_connect, frlan->get_height() - bh, bw_connect, bh, btn_connect, static_play_connect_lan_click, this);
    create_button(frlan, 0, frlan->get_height() - bh, bw_refresh, bh, btn_refresh, static_play_refresh_lan_click, this);

    title_bar = play_lan_list->get_title_bar();
    title_bar->add_column(i18n(I18N_MAINMENU_PROTOCOL), 40);
    title_bar->add_column(resources.get_icon("lock"), 9, "", 16);
    title_bar->add_column(i18n(I18N_MAINMENU_PLAYERS), 50);
    title_bar->add_column(i18n(I18N_MAINMENU_PING), 40);

    title_bar->get_columns()[0].addon = AscendingArrow;

    /* internet */
    GuiFrame *frwan = tab->create_tab(i18n(I18N_MAINMENU_INTERNET));
	play_wan_compatible = create_checkbox(frwan, 0, 0, i18n(I18N_MAINMENU_COMPATIBLE_SERVERS), compatible, on_compatible_click, this);
    create_label(frwan, 0, 15, i18n(I18N_MAINMENU_INTERNET_SERVERS));
    play_wan_list = create_listbox(frwan, 0, 30, frwan->get_width(), frwan->get_height() + 2 - (30 + bh + Spc), 0, FlagWidth, i18n(I18N_MAINMENU_SERVER_NAME), static_on_wan_entry_click, this);
    play_wan_list->show_title_bar(true);
    play_wan_list->set_on_title_clicked(static_play_wan_sort_click, this);

    create_button(frwan, frwan->get_width() - bw_connect, frwan->get_height() - bh, bw_connect, bh, btn_connect, static_play_connect_wan_click, this);
    create_button(frwan, 0, frwan->get_height() - bh, bw_refresh, bh, btn_refresh, static_play_refresh_wan_click, this);

    title_bar = play_wan_list->get_title_bar();
    title_bar->add_column(i18n(I18N_MAINMENU_PROTOCOL), 40);
    title_bar->add_column(resources.get_icon("lock"), 9, "", 16);
    title_bar->add_column(i18n(I18N_MAINMENU_PLAYERS), 50);
    title_bar->add_column(i18n(I18N_MAINMENU_PING), 40);

    title_bar->get_columns()[0].addon = AscendingArrow;

    /* custom server */
    GuiFrame *frcustom = tab->create_tab(i18n(I18N_MAINMENU_CUSTOM_SERVER));
    create_label(frcustom, 0, 0, i18n(I18N_MAINMENU_ENTER_HOSTNAME));
    create_label(frcustom, 0, 21, i18n(I18N_MAINMENU_ADDRESS));
    custom_ipaddress = create_textbox(frcustom, 90, 20, tab->get_width() - (2 * Spc) - 90 + 4, config.get_string("last_custom_address"));
    create_label(frcustom, 0, 41, i18n(I18N_MAINMENU_PORT));
    custom_port = create_textbox(frcustom, 90, 40, 100, config.get_string("last_custom_port"));
    custom_port->set_type(GuiTextbox::TypeInteger);
    create_label(frcustom, 0, 61, i18n(I18N_MAINMENU_PASSWORD));
    custom_password = create_textbox(frcustom, 90, 60, tab->get_width() - (2 * Spc) - 90 + 4, "");
    custom_password->set_type(GuiTextbox::TypeHidden);

    create_button(frcustom, frlan->get_width() - bw_connect, frlan->get_height() - bh, bw_connect, bh, btn_connect, static_play_manual, this);

    /* install sort handlers */
    wan_list_selected_entry = 0;
    lan_list_selected_entry = 0;
}

bool MainMenu::static_on_tab_click(GuiTab *sender, int index, void *data) {
    (reinterpret_cast<MainMenu *>(data))->create_server_locator(index);

    return true;
}

void MainMenu::static_on_wan_entry_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<MainMenu *>(data))->on_wan_entry_click(index);
}

void MainMenu::on_wan_entry_click(int index) {
    wan_list_selected_entry = play_wan_list->get_entry(index)->get_ptr_tag();
}

void MainMenu::static_on_lan_entry_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<MainMenu *>(data))->on_lan_entry_click(index);
}

void MainMenu::on_lan_entry_click(int index) {
    lan_list_selected_entry = play_lan_list->get_entry(index)->get_ptr_tag();
}

void MainMenu::static_cancel_click(GuiObject *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_close();
}

void MainMenu::static_play_close(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_close();
}

void MainMenu::play_close() {
    delete lan_broadcaster;
    delete master_query;
    lan_broadcaster = 0;
    master_query = 0;
    pop_window();
}

void MainMenu::static_play_wan_sort_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<MainMenu *>(data))->play_wan_sort_click(index);
}

void MainMenu::play_wan_sort_click(int index) {
    if (master_query) {
        ServerSort dir = master_query->sort(index);
        GuiListboxEntry::Columns& columns = play_wan_list->get_title_bar()->get_columns();
        for (GuiListboxEntry::Columns::iterator it = columns.begin(); it != columns.end(); it++) {
            it->addon = "";
        }
        columns[index].addon = (dir == Ascending ? AscendingArrow : DescendingArrow);
    }
}

void MainMenu::static_play_lan_sort_click(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<MainMenu *>(data))->play_lan_sort_click(index);
}

void MainMenu::play_lan_sort_click(int index) {
    if (lan_broadcaster) {
        ServerSort dir = lan_broadcaster->sort(index);
        GuiListboxEntry::Columns& columns = play_lan_list->get_title_bar()->get_columns();
        for (GuiListboxEntry::Columns::iterator it = columns.begin(); it != columns.end(); it++) {
            it->addon = "";
        }
        columns[index].addon = (dir == Ascending ? AscendingArrow : DescendingArrow);
    }
}

void MainMenu::static_play_connect_lan_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_connect_lan_click();
}

void MainMenu::play_connect_lan_click() {
    int index = play_lan_list->get_selected_index();
    const Hosts& hosts = lan_broadcaster->get_hosts();
    int sz = static_cast<int>(hosts.size());

    if (index >= 0 && index < sz) {
        const GameserverInformation *info = static_cast<const GameserverInformation *>(play_lan_list->get_entry(index)->get_ptr_tag());
        hostaddr_t host = info->host;
        hostport_t port = info->port;
        std::string pwd;
        if (info->secured) {
            if (show_inputbox(i18n(I18N_MAINMENU_ENTER_PASSWORD), pwd, true) != MessageBoxResponseYes) {
                return;
            }
        }
        if (info->protocol_version != ProtocolVersion) {
            show_messagebox(Gui::MessageBoxIconError, i18n(I18N_VERSION_MISMATCH_TITLE), i18n(I18N_VERSION_MISMATCH_MESSAGE));
        } else {
            BroadcasterGuard bg(lan_broadcaster, master_query);
            try {
                ScopeMusicStopper stop_music(subsystem, title_music);
                Client client(resources, subsystem, host, port, config, pwd);
                client.link_mouse(*this);
                client.run();
            } catch (const Exception& e) {
                show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
            }
        }
    } else {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_SERVER_TITLE), i18n(I18N_SELECT_SERVER));
    }
}

void MainMenu::static_play_refresh_lan_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_refresh_lan_click();
}

void MainMenu::play_refresh_lan_click() {
    Scope<Mutex> lock(lan_broadcaster->get_mutex());
    lan_list_selected_entry = 0;
    play_lan_list->set_selected_index(-1);
    lan_broadcaster->clear();
    lan_broadcaster->refresh();
    get_now(last_lan_info);
}

void MainMenu::static_play_refresh_wan_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_refresh_wan_click();
}

void MainMenu::play_refresh_wan_click() {
    Scope<Mutex> lock(master_query->get_mutex());
    wan_list_selected_entry = 0;
    play_wan_list->set_selected_index(-1);
    master_query->refresh();
}

void MainMenu::static_play_connect_wan_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_connect_wan_click();
}

void MainMenu::play_connect_wan_click() {
    int index = play_wan_list->get_selected_index();
    const Hosts& hosts = master_query->get_hosts();
    int sz = static_cast<int>(hosts.size());

    if (index >= 0 && index < sz) {
        const MasterQueryClient *info = static_cast<const MasterQueryClient *>(play_wan_list->get_entry(index)->get_ptr_tag());
        std::string pwd;
        if (info->secured) {
            if (show_inputbox(i18n(I18N_MAINMENU_ENTER_PASSWORD), pwd, true) != MessageBoxResponseYes) {
                return;
            }
        }
        hostaddr_t host = info->host;
        hostport_t port = info->port;
        if (info->protocol_version != ProtocolVersion) {
            show_messagebox(Gui::MessageBoxIconError, i18n(I18N_VERSION_MISMATCH_TITLE), i18n(I18N_VERSION_MISMATCH_MESSAGE));
        } else {
            BroadcasterGuard bg(lan_broadcaster, master_query);
            try {
                ScopeMusicStopper stop_music(subsystem, title_music);
                Client client(resources, subsystem, host, port, config, pwd);
                client.link_mouse(*this);
                client.run();
            } catch (const Exception& e) {
                show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
            }
        }
    } else {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_SERVER_TITLE), i18n(I18N_SELECT_SERVER));
    }
}

void MainMenu::static_play_manual(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->play_manual();
}

void MainMenu::play_manual() {
    hostaddr_t host = resolve_host(custom_ipaddress->get_text());
    hostport_t port = atoi(custom_port->get_text().c_str());

    /* tests */
    if (!host) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_SERVER_TITLE), i18n(I18N_SERVER_UNKNOWN));
        custom_ipaddress->set_focus();
        return;
    }
    if (!port) {
        show_messagebox(Gui::MessageBoxIconExclamation, i18n(I18N_PORT_TITLE), i18n(I18N_INVALID_PORT));
        custom_port->set_focus();
        return;
    }

    /* persist */
    config.set_string("last_custom_address", custom_ipaddress->get_text());
    config.set_int("last_custom_port", static_cast<int>(port));

    /* go */
    try {
        ScopeMusicStopper stop_music(subsystem, title_music);
        Client client(resources, subsystem, host, port, config, custom_password->get_text());
        client.link_mouse(*this);
        client.run();
    } catch (const Exception& e) {
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
    }
}

void MainMenu::on_compatible_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MainMenu *>(data))->compatible_click(state);
}

void MainMenu::compatible_click(bool state) {
    play_lan_compatible->set_state(state);
    play_wan_compatible->set_state(state);
    config.set_bool("compatible_servers_only", state);
}

/* ************************************************************************** */
/* Create server                                                              */
/* ************************************************************************** */
void MainMenu::static_create_server_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->create_server_click();
}

void MainMenu::create_server_click() {
    Font *f = get_font();
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 347 + 15;
    int wh = 308 + 21;
    GuiWindow *window = push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_MAINMENU_LAN));
    window->set_cancelable(true);

    /* tab container */
    int wcw = window->get_client_width();
    int wch = window->get_client_height();
    creation_tab = create_tab(window, Spc, Spc, wcw - (2 * Spc), wch - 25 - (2 * Spc));

    /* *** SERVER *** */
    GuiFrame *frserver = creation_tab->create_tab(i18n(I18N_MAINMENU_CREATE_SERVER1));
    create_label(frserver, 0, 1, i18n(I18N_MAINMENU_SERVER_NAME2));
    cs_server_name = create_textbox(frserver, 95, 0, 221, config.get_string("server_name"));
    cs_server_name->set_focus();

    create_label(frserver, 0, 21, i18n(I18N_MAINMENU_PORT));
    cs_server_port = create_textbox(frserver, 95, 20, 55, config.get_string("port"));
    cs_server_port->set_type(GuiTextbox::TypeInteger);
    cs_server_public = create_checkbox(frserver, 95 + cs_server_port->get_width() + 5, 22, i18n(I18N_MAINMENU_PUBLIC_SERVER), config.get_bool("public_server"), 0, 0);

    create_label(frserver, 0, 41, i18n(I18N_MAINMENU_PASSWORD));
    cs_server_password = create_textbox(frserver, 95, 40, 221 - 13, config.get_string("server_password"));
    cs_server_password->set_type(GuiTextbox::TypeHidden);
    cb_server_show_pwd = create_checkbox(frserver, 316 - 10, 42, "", false, static_show_pwd_click, this);

    create_box(frserver, 0, 43 + 20, frserver->get_width(), 1);

    /* game modes */
    int game_mode = config.get_int("game_mode");
    create_label(frserver, 0, 69, i18n(I18N_MAINMENU_GAME_MODE));
    cs_dm = create_checkbox(frserver, 95, 70, i18n(I18N_MAINMENU_GM_DM), game_mode == GamePlayTypeDM, static_game_mode_click, this);
    cs_tdm = create_checkbox(frserver, 95, 83, i18n(I18N_MAINMENU_GM_TDM), game_mode == GamePlayTypeTDM, static_game_mode_click, this);
    cs_ctf = create_checkbox(frserver, 95, 96, i18n(I18N_MAINMENU_GM_CTF), game_mode == GamePlayTypeCTF, static_game_mode_click, this);

    cs_sr = create_checkbox(frserver, 215, 70, i18n(I18N_MAINMENU_GM_SR), game_mode == GamePlayTypeSR, static_game_mode_click, this);
    cs_ctc = create_checkbox(frserver, 215, 83, i18n(I18N_MAINMENU_GM_CTC), game_mode == GamePlayTypeCTC, static_game_mode_click, this);
    cs_goh = create_checkbox(frserver, 215, 96, i18n(I18N_MAINMENU_GM_GOH), game_mode == GamePlayTypeGOH, static_game_mode_click, this);

    cs_current_mode = cs_dm;
    if (game_mode == GamePlayTypeTDM) {
        cs_current_mode = cs_tdm;
    } else if (game_mode == GamePlayTypeCTF) {
        cs_current_mode = cs_ctf;
    } else if (game_mode == GamePlayTypeSR) {
        cs_current_mode = cs_sr;
    } else if (game_mode == GamePlayTypeCTC) {
        cs_current_mode = cs_ctc;
    } else if (game_mode == GamePlayTypeGOH) {
        cs_current_mode = cs_goh;
    }

    cs_dm->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_dm->set_tag(GamePlayTypeDM);

    cs_tdm->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_tdm->set_tag(GamePlayTypeTDM);

    cs_ctf->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_ctf->set_tag(GamePlayTypeCTF);

    cs_sr->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_sr->set_tag(GamePlayTypeSR);

    cs_ctc->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_ctc->set_tag(GamePlayTypeCTC);

    cs_goh->set_style(GuiCheckbox::CheckBoxStyleCircle);
    cs_goh->set_tag(GamePlayTypeGOH);

    create_box(frserver, 0, 113, frserver->get_width(), 1);

    /* map selector */
    Icon *no_map = resources.get_icon("map_preview");
    cs_map_preview = create_picture(frserver, creation_tab->get_width() - 83, 136, no_map->get_tile()->get_tilegraphic());

    try {
        Icon *map_border = resources.get_icon("map_border");
        create_picture(frserver, creation_tab->get_width() - 83, 136, map_border->get_tile()->get_tilegraphic());
    } catch (...) {
        /* chomp */
    }

    cs_map_name = create_label(frserver, creation_tab->get_width() - 83, 135 + 68, "");
    cs_map_name->set_clip_width(64);

    create_label(frserver, 0, 119, i18n(I18N_MAINMENU_SELECT_MAP));
    int lbh = (get_font()->get_font_height()) * 6 + 2;
    cs_maps = create_listbox(frserver, 0, 135, 243, lbh, "", static_map_selected, this);

    fill_map_listbox(static_cast<GamePlayType>(game_mode));

    /* *** SETTINGS *** */
    GuiFrame *frsettings = creation_tab->create_tab(i18n(I18N_MAINMENU_CREATE_SERVER2));

    /* max players and duration */
    create_label(frsettings, 0, 1, i18n(I18N_MAINMENU_MAX_PLAYERS));
    cs_max_players = create_textbox(frsettings, 95, 0, 55, config.get_string("num_players"));
    cs_max_players->set_type(GuiTextbox::TypeInteger);

    create_label(frsettings, 0, 21, i18n(I18N_MAINMENU_WARMUP));
    cs_warmup = create_textbox(frsettings, 95, 20, 55, config.get_string("warmup"));
    cs_warmup->set_type(GuiTextbox::TypeInteger);
    create_label(frsettings, 155, 21, i18n(I18N_MAINMENU_IN_SECONDS));

    create_label(frsettings, 0, 41, i18n(I18N_MAINMENU_DURATION));
    cs_duration = create_textbox(frsettings, 95, 40, 55, config.get_string("duration"));
    cs_duration->set_type(GuiTextbox::TypeInteger);
    create_label(frsettings, 155, 41, i18n(I18N_MAINMENU_IN_MINUTES));

    create_box(frsettings, 0, 63, frserver->get_width(), 1);

    /* server settings */
    cs_opt_hold_disconnected_players = create_checkbox(frsettings, 0, 70, i18n(I18N_MAINMENU_HOLD_DISCONNECTED_PLAYERS), config.get_bool("hold_disconnected_player"), 0, 0);
    create_label(frsettings, 0, 86, i18n(I18N_MAINMENU_RECONNECT_KILLS1));
    cs_opt_reconnect_kills = create_textbox(frsettings, 95, 85, 55, config.get_string("reconnect_kills"));
    cs_opt_reconnect_kills->set_type(GuiTextbox::TypeInteger);

    create_box(frsettings, 0, 107, frserver->get_width(), 1);

    cs_opt_friendly_fire = create_checkbox(frsettings, 0, 114, i18n(I18N_MAINMENU_FRIENDLY_FIRE), config.get_bool("friendly_fire_alarm"), 0, 0);
    cs_opt_shooting_explosives = create_checkbox(frsettings, 0, 129, i18n(I18N_MAINMENU_SHOOT_EXPLOSIVES), config.get_bool("shot_explosives"), 0, 0);
    cs_opt_prevent_pick = create_checkbox(frsettings, 0, 144, i18n(I18N_MAINMENU_PREVENT_PICK), config.get_bool("prevent_pick"), 0, 0);
    cs_opt_refuse_join = create_checkbox(frsettings, 0, 159, i18n(I18N_MAINMENU_REFUSE_JOIN_IN_GAME), config.get_bool("refuse_join"), 0, 0);

    /* buttons */
    std::string btn_start(i18n(I18N_BUTTON_START_SERVER));
    int bw_start = f->get_text_width(btn_start) + 24;
    create_button(window, 12, wh - 46, bw_start, 18, btn_start, static_start_server_click, this);

    std::string text(i18n(I18N_BUTTON_CLOSE));
    int bw = f->get_text_width(text) + 24;
    create_button(window, bw_start + 20, wh - 46, bw, 18, text, static_close_start_server_click, this);

    text = i18n(I18N_BUTTON_CANCEL);
    bw = f->get_text_width(text) + 24;
    create_button(window, ww - bw - 14, wh - 46, bw, 18, text, static_close_window_click, this);
}

bool MainMenu::MapEntryCompare(const MapEntry& lhs, const MapEntry& rhs) {
    return lhs.map->get_description() < rhs.map->get_description();

}
void MainMenu::fill_map_listbox(GamePlayType game_play_type) {
    const std::string& map_name = config.get_string("map_name");
    int select_index = 0;
    Resources::ResourceObjects& maps = resources.get_maps();
    MapEntries map_entries;
    size_t sz = maps.size();
    cs_maps->clear();

    for (size_t i = 0; i < sz; i++) {
        Map *map = static_cast<Map *>(maps[i].object);
        if (map_is_valid(game_play_type, map->get_game_play_type())) {
            map_entries.push_back(MapEntry(map, i));
        }
    }
    std::sort(map_entries.begin(), map_entries.end(), MapEntryCompare);

    for (MapEntries::iterator it = map_entries.begin(); it != map_entries.end(); it++) {
        MapEntry& map_entry = *it;
        cs_maps->add_entry(map_entry.map->get_description())->set_tag(map_entry.index);
        if (map_entry.map->get_name() == map_name) {
            select_index = static_cast<int>(cs_maps->get_entry_count() - 1);
        }
    }

    if (maps.size()) {
        map_selected(select_index);
        cs_maps->set_top_index(select_index);
        cs_maps->set_selected_index(select_index);
    }
}

void MainMenu::static_show_pwd_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MainMenu *>(data))->show_pwd_click(state);
}

void MainMenu::show_pwd_click(bool state) {
    if (state) {
        cs_server_password->set_type(GuiTextbox::TypeNormal);
    } else {
        cs_server_password->set_type(GuiTextbox::TypeHidden);
    }
}

void MainMenu::static_game_mode_click(GuiCheckbox *sender, void *data, bool state) {
    (reinterpret_cast<MainMenu *>(data))->game_mode_click(sender);
}

void MainMenu::game_mode_click(GuiCheckbox *sender) {
    cs_current_mode = sender;
    cs_dm->set_state(false);
    cs_tdm->set_state(false);
    cs_ctf->set_state(false);
    cs_sr->set_state(false);
    cs_ctc->set_state(false);
    cs_goh->set_state(false);
    sender->set_state(true);
    fill_map_listbox(static_cast<GamePlayType>(sender->get_tag()));
}

void MainMenu::static_map_selected(GuiListbox *sender, void *data, int index) {
    (reinterpret_cast<MainMenu *>(data))->map_selected(index);
}

void MainMenu::map_selected(int index) {
    int map_index = cs_maps->get_entry(index)->get_tag();
    Map *map = static_cast<Map *>(resources.get_maps()[map_index].object);
    cs_map_name->set_caption(map->get_name());
    Tile *preview = map->get_preview();
    if (preview) {
        cs_map_preview->set_picture(preview->get_tilegraphic());
    } else {
        cs_map_preview->set_picture(resources.get_icon("map_preview")->get_tile()->get_tilegraphic());
    }
}

void MainMenu::static_start_server_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->server_validate(false);
}

void MainMenu::static_close_start_server_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->server_validate(true);
}

void MainMenu::server_validate(bool close) {
    if (!cs_server_name->get_text().length()) {
        creation_tab->select_tab(0);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_SERVER_NAME), i18n(I18N_MAINMENU_INVALID_SERVERNAME));
        cs_server_name->set_focus();
        return;
    }

    int port = atoi(cs_server_port->get_text().c_str());
    if (port < 1 || port > 65535) {
        creation_tab->select_tab(0);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_PORT_TITLE), i18n(I18N_INVALID_PORT));
        cs_server_port->set_focus();
        return;
    }

    bool public_server = cs_server_public->get_state();

    int game_mode = static_cast<GamePlayType>(cs_current_mode->get_tag());

    int max_players = atoi(cs_max_players->get_text().c_str());
    if (max_players < 2 || max_players > 64) {
        creation_tab->select_tab(1);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_MAX_PLAYERS_TITLE), i18n(I18N_MAINMENU_INVALID_GENERIC_NUMBER, "2 - 64"));
        cs_max_players->set_focus();
        return;
    }

    int duration = atoi(cs_duration->get_text().c_str());
    if (duration < 2 || duration > 240) {
        creation_tab->select_tab(1);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_DURATION2), i18n(I18N_MAINMENU_INVALID_GENERIC_NUMBER, "2 - 240"));
        cs_duration->set_focus();
        return;
    }

    int warmup = atoi(cs_warmup->get_text().c_str());
    if (warmup < 0 || warmup > 60) {
        creation_tab->select_tab(1);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_WARMUP2), i18n(I18N_MAINMENU_INVALID_GENERIC_NUMBER, "0 - 60"));
        cs_warmup->set_focus();
        return;
    }

    int reconnect_kills = atoi(cs_opt_reconnect_kills->get_text().c_str());
    if (reconnect_kills < 0 || reconnect_kills > 100) {
        creation_tab->select_tab(1);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_RECONNECT_KILLS2), i18n(I18N_MAINMENU_INVALID_GENERIC_NUMBER, "0 - 100"));
        cs_opt_reconnect_kills->set_focus();
        return;
    }

    int selected_map = cs_maps->get_entry(cs_maps->get_selected_index())->get_tag();
    Resources::ResourceObjects& maps = resources.get_maps();
    int sz = static_cast<int>(maps.size());
    if (selected_map < 0 || selected_map >= sz) {
        creation_tab->select_tab(0);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_MAINMENU_SELECT_MAP2), i18n(I18N_MAINMENU_SELECT_MAP3));
        cs_maps->set_focus();
        return;
    }
    Map *map = static_cast<Map *>(maps[selected_map].object);

    /* game mode map match */
    if (!map_is_valid(static_cast<GamePlayType>(game_mode), map->get_game_play_type())) {
        creation_tab->select_tab(0);
        show_messagebox(Gui::MessageBoxIconError, i18n(I18N_WRONG_MAPTYPE), i18n(I18N_WRONG_MAPTYPE2));
        cs_maps->set_focus();
        return;
    }

    /* persist */
    config.set_string("server_name", cs_server_name->get_text());
    config.set_int("port", port);
    config.set_bool("public_server", public_server);
    config.set_string("server_password", cs_server_password->get_text());
    config.set_int("game_mode", game_mode);
    config.set_int("num_players", max_players);
    config.set_int("duration", duration);
    config.set_int("warmup", warmup);
    config.set_string("map_name", map->get_name());
    config.set_bool("friendly_fire_alarm", cs_opt_friendly_fire->get_state());
    config.set_bool("shot_explosives", cs_opt_shooting_explosives->get_state());
    config.set_bool("prevent_pick", cs_opt_prevent_pick->get_state());
    config.set_bool("refuse_join", cs_opt_refuse_join->get_state());
    config.set_bool("hold_disconnected_player", cs_opt_hold_disconnected_players->get_state());
    config.set_int("reconnect_kills", reconnect_kills);

    /* start server or close window */
    if (close) {
        pop_window();
    } else {
        try {
            ScopeMusicStopper stop_music(subsystem, title_music);
            GamePlayType type = static_cast<GamePlayType>(game_mode);
            Server server(resources, subsystem, config.get_key_value(), type,
                config.get_string("map_name"), duration, warmup, public_server
            );
            ScopeServer scope_server(server);
            Client client(resources, subsystem, INADDR_LOOPBACK, port, config, config.get_string("server_password"));
            client.link_mouse(*this);
            client.run();
        } catch (const Exception& e) {
            show_messagebox(Gui::MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
        }
        fill_map_listbox(static_cast<GamePlayType>(cs_current_mode->get_tag()));
    }
}

bool MainMenu::map_is_valid(GamePlayType selected_type, GamePlayType map_type) {
    if (selected_type == GamePlayTypeSR || map_type == GamePlayTypeSR ||
        selected_type == GamePlayTypeCTC || map_type == GamePlayTypeCTC ||
        selected_type == GamePlayTypeGOH || map_type == GamePlayTypeGOH)
    {
        return (selected_type == map_type);
    }

    return (selected_type <= map_type);
}

/* ************************************************************************** */
/* List Packages                                                              */
/* ************************************************************************** */
void MainMenu::static_list_packages_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->list_packages_click();
}

void MainMenu::list_packages_click() {
    Font *f = get_font();
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 380;
    int wh = 250;
    int bh = 18;
    const int hash_width = 110;

    GuiWindow *window = push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_MAINMENU_PACKAGES));
    window->set_cancelable(true);
    wh = window->get_client_height() + 2;

    GuiListbox *lb = create_listbox(window, Spc, Spc, ww - 2 * Spc, wh - 3 * Spc - bh + 3, i18n(I18N_MAINMENU_PACKAGE), 0, 0);
    lb->show_title_bar(true);
    GuiListboxEntry *titlebar = lb->get_title_bar();
    titlebar->add_column(i18n(I18N_MAINMENU_HASH), hash_width);

    const Resources::LoadedPaks loaded_paks = resources.get_loaded_paks();
    for (Resources::LoadedPaks::const_iterator it = loaded_paks.begin(); it != loaded_paks.end(); it++) {
        const Resources::LoadedPak& pak = *it;
        GuiListboxEntry *entry = lb->add_entry(pak.pak_short_name);
        entry->add_column(pak.pak_hash, hash_width);
    }
    lb->set_selected_index(0);

    /* close button */
    std::string text(i18n(I18N_BUTTON_CLOSE));
    int bw = f->get_text_width(text) + 28;
    create_button(window, ww / 2 - bw / 2, wh - bh - Spc, bw, bh, text, static_close_packages_list_click, this);
}

void MainMenu::static_close_packages_list_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->close_packages_list_click();
}
void MainMenu::close_packages_list_click() {
    pop_window();
}

/* ************************************************************************** */
/* Options                                                                    */
/* ************************************************************************** */
void MainMenu::static_options_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->options_click();
}

void MainMenu::options_click() {
    show_options();
}

/* ************************************************************************** */
/* Credits                                                                    */
/* ************************************************************************** */
void MainMenu::static_credits_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->credits_click();
}

void MainMenu::credits_click() {
    int vw = subsystem.get_view_width();
    int vh = subsystem.get_view_height();
    int ww = 220;
    int wh = 178;
    int lft = 25;
    int tab = 87;

    GuiWindow *window = push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, i18n(I18N_MAINMENU_CREDITS));
    window->set_cancelable(true);

    create_label(window, lft, 10, i18n(I18N_MAINMENU_CREDITS_CODE));
    create_label(window, tab, 10, "freanux");

    create_label(window, lft, 30, i18n(I18N_MAINMENU_CREDITS_GFX));
    create_label(window, tab, 30, "freanux, ruby, cataclisma");

    create_label(window, lft, 50, i18n(I18N_MAINMENU_CREDITS_MAPS));
    create_label(window, tab, 50, "ruby, freanux, cataclisma");

    int y = 70;

    if (title_music) {
        create_label(window, lft, y, i18n(I18N_MAINMENU_CREDITS_MUSIC));
        create_label(window, tab, y, "Daniel Wressle");
        y += 15;
        wh += 15;
    }

    Font *f = get_font();
    y += 15;

    std::string t1(i18n(I18N_MAINMENU_CREDITS_THANKS));
    int x1 = (ww - 2) / 2 - f->get_text_width(t1) / 2;
    std::string t2("ruby, cataclisma, julia, harambe, bier");
    int x2 = (ww - 2) / 2 - f->get_text_width(t2) / 2;
    std::string t3(i18n(I18N_MAINMENU_FREDERIC));
    int x3 = (ww - 2) / 2 - f->get_text_width(t3) / 2;

    create_label(window, x1, y, t1);
    create_label(window, x2, y + 15, t2);
    create_label(window, x3, y + 30, t3);

    window->set_height(wh);
    window->set_y(vh / 2 - wh / 2);

    std::string text(i18n(I18N_BUTTON_CLOSE));
    int bw = get_font()->get_text_width(text) + 28;
    create_button(window, ww / 2 - bw / 2, wh - 43, bw, 18, text, static_close_window_click, this);
}

/* ************************************************************************** */
/* Quit                                                                       */
/* ************************************************************************** */
void MainMenu::static_quit_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->leave();
}

/* ************************************************************************** */
/* Window close                                                               */
/* ************************************************************************** */
void MainMenu::static_close_window_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<MainMenu *>(data))->close_window_click();
}

void MainMenu::close_window_click() {
    pop_window();
}

/* ************************************************************************** */
/* Helpers                                                                    */
/* ************************************************************************** */
void MainMenu::create_server_locator(int index) {
    try {
        switch (index) {
            case 0: // LAN
                if (!lan_broadcaster) {
                    lan_broadcaster = new LANBroadcaster(i18n, config.get_int("port"));
                    get_now(last_lan_info);
                }
                break;
            case 1: // internet
                if (!master_query) {
                    master_query = new MasterQuery(i18n, config.get_string("master_server"), config.get_int("master_port"));
                }
                break;
        }
    } catch (const Exception& e) {
        show_messagebox(MessageBoxIconError, i18n(I18N_ERROR_TITLE), e.what());
    }
}

void MainMenu::destroy_server_locators() {
    if (lan_broadcaster) {
        delete lan_broadcaster;
        lan_broadcaster = 0;
    }

    if (master_query) {
        delete master_query;
        master_query = 0;
    }
}

void MainMenu::static_lang_change_cb(void *data) {
    reinterpret_cast<MainMenu *>(data)->change_button_captions();
}

void MainMenu::change_button_captions() {
    for (int i = 0; i < 6; i++) {
        if (rb[i]) {
            std::string caption;
            switch (i) {
                case 0:
                    caption = i18n(I18N_MAINMENU_PLAY);
                    break;

                case 1:
                    caption = i18n(I18N_MAINMENU_LAN);
                    break;

                case 2:
                    caption = i18n(I18N_MAINMENU_PACKAGES);
                    break;

                case 3:
                    caption = i18n(I18N_MAINMENU_OPTIONS);
                    break;

                case 4:
                    caption = i18n(I18N_MAINMENU_CREDITS);
                    break;

                case 5:
                    caption = i18n(I18N_MAINMENU_QUIT);
                    break;
            }
            rb[i]->set_caption(caption);
        }
    }
    set_version_label();
}

void MainMenu::set_version_label() {
    main_window->remove_object(version_label);
    main_window->remove_object(beta);

    Font *f = get_font();
    int th = f->get_font_height();
    std::string version(i18n(I18N_MAINMENU_VERSION, GameVersion));
    int tw = f->get_text_width(version);
    version_label = create_label(main_window, mw_w / 2 - tw / 2, mw_h - th - 21, version);
    version_label->set_follow_alpha(false);

    if (ProductIsBeta) {
        Icon *beta_icon = resources.get_icon("beta");
        create_picture(main_window, mw_w / 2 + tw / 2, mw_h - 32 - Spc - 3, beta_icon->get_tile()->get_tilegraphic())->set_follow_alpha(false);
    }
}
