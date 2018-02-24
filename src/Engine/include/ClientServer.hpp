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


#ifndef _CLIENTSERVER_HPP_
#define _CLIENTSERVER_HPP_

#include "MessageSequencer.hpp"
#include "Tournament.hpp"
#include "Player.hpp"
#include "GameProtocol.hpp"
#include "TournamentFactory.hpp"
#include "MapConfiguration.hpp"
#include "I18N.hpp"

#include <string>

class Server;

class ClientServer : public MessageSequencer {
private:
    ClientServer(const ClientServer&);
    ClientServer& operator=(const ClientServer&);

public:
    ClientServer(I18N& i18n, hostport_t port, pico_size_t num_players,
        const std::string& server_name, const std::string& password);
    ClientServer(I18N& i18n, hostaddr_t host, hostport_t port);
    ~ClientServer();

    Tournament *get_tournament() const;
    Players& get_players();
    void delete_tournament();
    bool use_temporary_map_config() const;
    void set_temporary_map_config(bool state);
    MapConfiguration& get_temporary_map_config();

protected:
    Server *server;
    Tournament *tournament;
    GTransport *gtrans;

    Players players;

#ifdef SAFE_ALIGNMENT
    data_t aligned_buffer[1024];
#endif

public:
    void stacked_send_data(const Connection *c, unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data);

    void stacked_broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data);

    void stacked_broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data);

    void flush_stacked_send_data(const Connection *c, flags_t flags);
    void flush_stacked_broadcast_data(flags_t flags);
    void flush_stacked_broadcast_data_synced(flags_t flags);

    void send_data(const Connection *c, unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data);

    void broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags, data_len_t len,
        const void *data);

    void broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags, data_len_t len,
        const void *data);

    void set_server(Server *server);
    void reload_config(hostport_t port, pico_size_t num_players,
        const std::string& server_name, const std::string& password);

    hostport_t get_port() const;

protected:
    char *create_text(const std::string& txt, size_t& sz);
    I18NText get_logout_text_id(LogoutReason reason);
    I18NText get_logout_text_id_client(LogoutReason reason);

private:
    char buffer[1024];
    GTransport *pb;
    int packet_len;
    hostport_t port;
    bool has_temp_map_config;

    MapConfiguration temp_map_config;

    void stack_data(unsigned char tournament_id, command_t cmd, data_len_t len, const void *data);
};

#endif
