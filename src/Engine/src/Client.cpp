#include "Client.hpp"
#include "Utils.hpp"
#include "Scope.hpp"

#include <iostream>
#include <algorithm>

static double TextMessageDuration = 7500.0f;
static double TextMessageFadeOutAt = 6000.0f;

const ns_t CycleS = 1000000000;
const int BroadcastsPerS = 15; //20;
const ns_t UpdatePeriod = CycleS / BroadcastsPerS;

template <class T> static bool erase_element(T *elem) {
    if (elem->delete_me) {
        delete elem;
        return true;
    }

    return false;
}

Client::Client(Resources& resources, Subsystem& subsystem, hostaddr_t host,
    hostport_t port, Configuration& config, const std::string& password)
    throw (Exception)
    : ClientServer(host, port),
      Gui(resources, subsystem, resources.get_font("normal")),
      OptionsMenu(*this, resources, subsystem, config, true),
      resources(resources), subsystem(subsystem), player_config(config),
      logged_in(false), me(0), updatecnt(0),
      factory(resources, subsystem, this), my_id(0), login_sent(false),
      throw_exception(false), exception_msg(), force_send(false),
      fhnd(0), running(true), reload_resources(true)
{
    conn = 0;
    get_now(last);

    /* start data receiver thread */
    if (!thread_start()) {
        throw ClientException("Starting client thread failed.");
    }

    /* login */
    memset(gplayerdesc, 0, sizeof(GPlayerDescription));
    strncpy(gplayerdesc->player_name, player_config.get_player_name().c_str(), NameLength - 1);
    strncpy(gplayerdesc->characterset_name, player_config.get_player_skin().c_str(), NameLength - 1);
    {
        Scope<Mutex> lock(mtx);
        login(password, GPlayerDescriptionLen, gplayerdesc);
    }
    binding.extract_from_config(player_config);

    /* start music player */
    subsystem.start_music_player(resources, *this);
}

Client::~Client() {
    /* stop music player */
    subsystem.stop_music_player();

    /* stop thread if running */
    if (running) {
        stop_thread();
    }

    /* cleanup server buffer */
    while (!server_events.empty()) {
        ServerEvent evt = server_events.front();
        server_events.pop();
        if (evt.data) {
            delete[] evt.data;
        }
    }

    /* cleanup */
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        delete *it;
    }

    /* close file if opened */
    if (fhnd) {
        fclose(fhnd);
    }

    /* delete partial downloaded file */
    if (current_download_filename.length()) {
        remove(current_download_filename.c_str());
    }
}

void Client::idle() throw (Exception) {
    /* process net io */
    bool queue_empty = true;
    do {
        ServerEvent evt;
        {
            Scope<Mutex> lock(mtx);
            if (server_events.empty()) {
                break;
            }
            evt = server_events.front();
            server_events.pop();
            queue_empty = server_events.empty();
        }
        switch (evt.event) {
            case EventTypeStatus:
            {
                std::string msg(evt.data, evt.sz);
                subsystem << msg << std::endl;
                break;
            }

            case EventTypeAccessDenied:
            {
                stop_thread();
                exception_msg.assign(evt.data, evt.sz);
                throw_exception = true;
                break;
            }

            case EventTypeLogin:
            {
                sevt_login(evt);
                break;
            }

            case EventTypeLogout:
            {
                stop_thread();
                logged_in = false;
                conn = 0;
                exception_msg.assign(evt.data, evt.sz);
                throw_exception = true;
                break;
            }

            case EventTypeData:
                sevt_data(evt);
                break;
        }
        if (evt.data) {
            delete[] evt.data;
        }
    } while (!queue_empty && !throw_exception);

    /* have to throw an exception? */
    if (throw_exception) {
        throw ClientException(exception_msg);
    }

    /* interpolate movements */
    get_now(now);
    ns_t diff = diff_ns(last, now);

    last = now;

    if (tournament) {
        tournament->update_states(diff);
        if (conn) {
            tournament->set_ping_time(conn->ping_time);
            Tournament::StateResponses& responses = tournament->get_state_responses();
            size_t sz = responses.size();
            {
                Scope<Mutex> lock(mtx);
                for (size_t i = 0; i < sz; i++) {
                    StateResponse *resp = responses[i];
                    if (resp->action == GPCTextMessage) {
                        std::string msg(reinterpret_cast<const char *>(resp->data), resp->len);
                        add_text_msg(msg);
                    } else {
                        stacked_send_data(conn, factory.get_tournament_id(), resp->action, 0, resp->len, resp->data);
                    }
                }
                flush_stacked_send_data(conn, 0);
            }
        }
        tournament->delete_responses();

        /* send player position (unreliable) */
        updatecnt += diff;
        bool player_force = false;
        if (me && me->force_broadcast) {
            player_force = true;
            me->force_broadcast = false;
            me->state.client_server_state.flags |= PlayerClientServerFlagForceBroadcast;
        }
        if (updatecnt >= UpdatePeriod || force_send || player_force) {
            updatecnt = 0;
            if (conn && me && tournament->is_ready()) {
                GPlayerClientServerState state;
                state = me->state.client_server_state;
                state.to_net();
                {
                    Scope<Mutex> lock(mtx);
                    send_data(conn, factory.get_tournament_id(), GPSUpdatePlayerClientServerState, 0, GPlayerClientServerStateLen, &state);
                }
                me->state.client_server_state.flags &= ~PlayerClientServerFlagForceBroadcast;
            }
        }
    }

    /* interpolate messages */
    for (TextMessages::iterator it = text_messages.begin();
        it != text_messages.end(); it++)
    {
        TextMessage *cmsg = *it;
        cmsg->duration += (diff / 1000000.0f);
        if (cmsg->duration > TextMessageDuration) {
            cmsg->delete_me = true;
        }
    }
    text_messages.erase(std::remove_if(text_messages.begin(),
        text_messages.end(), erase_element<TextMessage>),
        text_messages.end());

    /* player in options or in chatbox? */
    if (me) {
        if (get_stack_count()) {
            me->state.client_server_state.flags |= PlayerClientServerFlagWriting;
        } else {
            me->state.client_server_state.flags &= ~PlayerClientServerFlagWriting;
        }
    }

    /* draw map */
    if (tournament) {
        tournament->draw();
    }

    /* draw messages */
    Font *font = resources.get_font("normal");
    int view_height = subsystem.get_view_height();
    int font_height = font->get_font_height();
    int y = view_height - font_height - 5;
    for (TextMessages::reverse_iterator it = text_messages.rbegin();
        it != text_messages.rend(); it++)
    {
        TextMessage *cmsg = *it;
        float alpha = 1.0f;
        if (cmsg->duration > TextMessageFadeOutAt) {
            alpha = static_cast<float>((TextMessageDuration - cmsg->duration) / (TextMessageDuration - TextMessageFadeOutAt));
        }

        subsystem.set_color(0.75f, 0.75f, 1.0f, alpha);
        subsystem.draw_text(font, 5, y, cmsg->text);
        alpha *= 0.9f;
        y -= font_height;
    }
    subsystem.reset_color();

    /* draw file transfer status */
    if (fhnd) {
        Font *big = resources.get_font("big");
        int percent = 100 - static_cast<int>(100.0f / static_cast<float>(total_xfer_sz) * remaining_xfer_sz);
        sprintf(buffer, "transferring %s (%d%%)", xfer_filename.c_str(), percent);
        int tw = big->get_text_width(buffer);
        subsystem.draw_text(big, subsystem.get_view_width() / 2 - tw / 2, view_height - 30, buffer);
    }
}

void Client::set_key(MappedKey::Device dev, int param) {
    set_key(dev, param, binding.left, PlayerKeyStateLeft);
    set_key(dev, param, binding.right, PlayerKeyStateRight);
    set_key(dev, param, binding.up, PlayerKeyStateUp);
    set_key(dev, param, binding.down, PlayerKeyStateDown);
    set_key(dev, param, binding.jump, PlayerKeyStateJump);
    set_key(dev, param, binding.fire, PlayerKeyStateFire);
    set_key(dev, param, binding.drop1, PlayerKeyStateDrop1);
    set_key(dev, param, binding.drop2, PlayerKeyStateDrop2);
    set_key(dev, param, binding.drop3, PlayerKeyStateDrop3);

    if (binding.chat.device == dev && binding.chat.param == param) {
        if (!get_stack_count()) {
            subsystem.clear_input_buffer();
            int vw = subsystem.get_view_width();
            int vh = subsystem.get_view_height();
            int ww = 350;
            int wh = 30;
            GuiWindow *window = push_window(1, 1, ww, wh, "Enter Message");
            window->set_on_keydown(static_window_keydown, this);
            window->set_on_joybuttondown(static_window_joybutton_down, this);
            ww = window->get_client_width();
            chat_textbox = create_textbox(window, Spc, Spc, ww - 2 * Spc, "");
            window->set_height(window->get_height() - window->get_client_height() + 2 * Spc + chat_textbox->get_height());
            window->set_x(vw / 2 - window->get_client_width() / 2);
            window->set_y(vh / 2 - window->get_client_height() / 2);
            chat_textbox->set_focus();
            if (me) {
                me->state.client_server_state.key_states = 0;
            }
        }
    }

    if (binding.stats.device == dev && binding.stats.param == param) {
        if (!get_stack_count()) {
            if (tournament) {
                tournament->show_stats(true);
            }
        }
    }

    if (binding.escape.device == dev && binding.escape.param == param) {
        show_options_menu();
    }
}

void Client::set_key(MappedKey::Device dev, int param, MappedKey& key, int flag) {
    if (key.device == dev && key.param == param) {
        if (me) {
            me->state.client_server_state.key_states |= flag;
            force_send = true;
        }
    }
}

void Client::reset_key(MappedKey::Device dev, int param) {
    reset_key(dev, param, binding.left, PlayerKeyStateLeft);
    reset_key(dev, param, binding.right, PlayerKeyStateRight);
    reset_key(dev, param, binding.up, PlayerKeyStateUp);
    reset_key(dev, param, binding.down, PlayerKeyStateDown);
    reset_key(dev, param, binding.jump, PlayerKeyStateJump);
    reset_key(dev, param, binding.fire, PlayerKeyStateFire);
    reset_key(dev, param, binding.drop1, PlayerKeyStateDrop1);
    reset_key(dev, param, binding.drop2, PlayerKeyStateDrop2);
    reset_key(dev, param, binding.drop3, PlayerKeyStateDrop3);

    if (binding.stats.device == dev && binding.stats.param == param) {
        if (!get_stack_count()) {
            if (tournament) {
                tournament->show_stats(false);
            }
        }
    }
}

void Client::reset_key(MappedKey::Device dev, int param, MappedKey& key, int flag) {
    if (key.device == dev && key.param == param) {
        if (me) {
            me->state.client_server_state.key_states &= ~flag;
            force_send = true;
        }
    }
}

void Client::on_input_event(const InputData& input) {
    if (!are_options_visible()) {
        switch (input.data_type) {
            case InputData::InputDataTypeKeyDown:
                set_key(MappedKey::DeviceKeyboard, input.keycode);
                /* escape fallback */
                if (input.key_type == InputData::InputKeyTypeEscape) {
                    if (!input.key_repeat) {
                        show_options_menu();
                    }
                }
                break;

            case InputData::InputDataTypeKeyUp:
                reset_key(MappedKey::DeviceKeyboard, input.keycode);
                break;

            case InputData::InputDataTypeJoyButtonDown:
                set_key(MappedKey::DeviceJoyButton, input.param1);
                break;

            case InputData::InputDataTypeJoyButtonUp:
                reset_key(MappedKey::DeviceJoyButton, input.param1);
                break;

            case InputData::InputDataTypeJoyMotion:
            {
                joyaxis_t axis = static_cast<joyaxis_t>(input.param1);
                if (me) {
                    if (!get_stack_count()) {
                        me->state.client_server_state.jaxis = axis;
                    } else {
                        me->state.client_server_state.jaxis = 0;
                    }
                }
                break;
            }

            default:
                break;
        }
    }
}

void Client::on_leave() {
    subsystem.stop_music_player();
}

void Client::options_closed() {
    player_config.extract();
    binding.extract_from_config(player_config);

    if (old_player_name != player_config.get_player_name() ||
        old_player_skin != player_config.get_player_skin())
    {
        GPlayerDescription pdesc;
        memset(&pdesc, 0, GPlayerDescriptionLen);
        strncpy(pdesc.player_name, player_config.get_player_name().c_str(), NameLength - 1);
        strncpy(pdesc.characterset_name, player_config.get_player_skin().c_str(), NameLength - 1);
        {
            Scope<Mutex> lock(mtx);
            send_data(conn, factory.get_tournament_id(), GPSPlayerChanged, NetFlagsReliable, GPlayerDescriptionLen, &pdesc);
        }
    }
}

void Client::static_window_close_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Client *>(data))->window_close_click();
}

bool Client::static_window_keydown(GuiWindow *sender, void *data, int keycode, bool repeat) {
    return (reinterpret_cast<Client *>(data))->window_keydown(keycode);
}

bool Client::window_keydown(int keycode) {
    switch (keycode) {
        case 27:
            if (me) {
                me->state.client_server_state.flags &= ~PlayerClientServerFlagWriting;
            }
            window_close_click();
            return true;

        case 13:
            if (me) {
                me->state.client_server_state.flags &= ~PlayerClientServerFlagWriting;
            }
            chat_send_message();
            return true;
    }
    if (binding.escape.device == MappedKey::DeviceKeyboard && binding.escape.param == keycode) {
        if (me) {
            me->state.client_server_state.flags &= ~PlayerClientServerFlagWriting;
        }
        window_close_click();
        return true;
    }

    return false;
}

bool Client::static_window_joybutton_down(GuiWindow *sender, void *data, int button) {
    return (reinterpret_cast<Client *>(data))->window_joybutton_down(button);
}

bool Client::window_joybutton_down(int button) {
    if (binding.escape.device == MappedKey::DeviceJoyButton && binding.escape.param == button) {
        if (me) {
            me->state.client_server_state.flags &= ~PlayerClientServerFlagWriting;
        }
        window_close_click();
        return true;
    }

    return false;
}

void Client::window_close_click() {
    pop_window();
}

void Client::chat_send_message() {
    chat_textbox->set_text(trim(chat_textbox->get_text()));
    const std::string& text = chat_textbox->get_text();

    if (text.length() && conn) {
        {
            Scope<Mutex> lock(mtx);
            send_data(conn, factory.get_tournament_id(), GPSChatMessage, NetFlagsReliable, static_cast<data_len_t>(text.length()), text.c_str());
        }
    }
    window_close_click();
}

void Client::show_options_menu() {
    if (!get_stack_count()) {
        old_player_name = player_config.get_player_name();
        old_player_skin = player_config.get_player_skin();
        show_options();
        if (me) {
            me->state.client_server_state.key_states = 0;
        }
    }
}

void Client::thread() {
    /* net loop */
    while (running) {
        {
            Scope<Mutex> lock(mtx);
            while(cycle());
        }
        wait_ns(1000000);
    }

    /* gracefully logout */
    {
        Scope<Mutex> lock(mtx);
        logout();
        while(cycle());
    }
}

void Client::stop_thread() {
    running = false;
    thread_join();
}