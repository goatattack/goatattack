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

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "TextMessageSystem.hpp"
#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Thread.hpp"
#include "ClientServer.hpp"
#include "TournamentFactory.hpp"
#include "Gui.hpp"
#include "Configuration.hpp"
#include "OptionsMenu.hpp"
#include "KeyBinding.hpp"
#include "Mutex.hpp"

#include <string>
#include <vector>
#include <queue>
#include <cstdio>

class ClientException : public Exception {
public:
    ClientException(const char *msg) : Exception(msg) { }
    ClientException(std::string msg) : Exception(msg) { }
};

class Client
    : public ClientServer, public Gui, protected OptionsMenu,
      protected TextMessageSystem, protected Thread
{
private:
    Client(const Client&);
    Client& operator=(const Client&);

public:
    Client(Resources& resources, Subsystem& subsystem, hostaddr_t host,
        hostport_t port, Configuration& config, const std::string& password)
        throw (Exception);
    virtual ~Client();

    bool is_game_over() const;
    bool is_spectating() const;
    void spectate();
    void change_team();

    // Gui
    virtual void idle() throw (Exception);
    virtual void on_input_event(const InputData& input);

protected:
    virtual void on_leave();

private:
    enum EventType {
        EventTypeStatus = 0,
        EventTypeAccessDenied,
        EventTypeLogin,
        EventTypeLogout,
        EventTypeData
    };

    struct ServerEvent {
        ServerEvent() : event(EventTypeStatus), c(0), data(0), sz(0) { }
        ServerEvent(EventType event, const Connection *c, char *data, size_t sz) : event(event), c(c), data(data), sz(sz) { }

        EventType event;
        const Connection *c;
        char *data;
        size_t sz;
    };

    typedef std::queue<ServerEvent> ServerEvents;

    /* ctor */
    Resources& resources;
    Subsystem& subsystem;
    Configuration& player_config;
    bool logged_in;
    Player *me;
    ns_t updatecnt;
    TournamentFactory factory;
    player_id_t my_id;
    bool login_sent;
    bool throw_exception;
    std::string exception_msg;
    bool force_send;
    FILE *fhnd;
    bool running;
    bool reload_resources;

    double text_message_duration;
    double text_message_fade_out_at;

    gametime_t now;
    gametime_t last;

    /* hidden */
    const Connection *conn;
    GuiTextbox *chat_textbox;
    KeyBinding binding;
    std::string old_player_name;
    std::string old_player_skin;
    std::string team_red_name;
    std::string team_blue_name;

    datasize_t total_xfer_sz;
    datasize_t remaining_xfer_sz;
    std::string xfer_filename;
    char buffer[128];
    Mutex mtx;
    ServerEvents server_events;
    std::string current_download_filename;

    /* implements MessageSequencer */
    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version) throw (Exception);
    virtual void event_access_denied(MessageSequencer::RefusalReason reason) throw (Exception);
    virtual void event_login(const Connection *c, data_len_t len, void *data) throw (Exception);
    virtual void event_data(const Connection *c, data_len_t len, void *data) throw (Exception);
    virtual void event_logout(const Connection *c, LogoutReason reason) throw (Exception);

    /* implements OptionsMenu */
    virtual void options_closed();

    /* implements Thread */
    virtual void thread();

    /* server sync func */
    void stop_thread();
    void sevt_login(ServerEvent& evt);
    void sevt_data(ServerEvent& evt);

    /* gui funcs */
    void set_key(MappedKey::Device dev, int param);
    void set_key(MappedKey::Device dev, int param, MappedKey& key, int flag);
    void reset_key(MappedKey::Device dev, int param);
    void reset_key(MappedKey::Device dev, int param, MappedKey& key, int flag);

    static void static_window_close_click(GuiVirtualButton *sender, void *data);
    static bool static_window_keydown(GuiWindow *sender, void *data, int keycode, bool repeat);
    static bool static_window_joybutton_down(GuiWindow *sender, void *data, int button);
    bool window_keydown(int keycode);
    bool window_joybutton_down(int button);

    void window_close_click();
    void chat_send_message();
    void show_options_menu();
    void update_text_fade_speed();
    void update_options_window();
};

#endif
