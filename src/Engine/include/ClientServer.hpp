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

#include <string>

class Server;

class ClientServer : public MessageSequencer {
private:
    ClientServer(const ClientServer&);
    ClientServer& operator=(const ClientServer&);

public:
    ClientServer(hostport_t port, pico_size_t num_players,
        const std::string& server_name, const std::string& password);
    ClientServer(hostaddr_t host, hostport_t port);
    virtual ~ClientServer();

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

public:
    void stacked_send_data(const Connection *c, unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data) throw (Exception);

    void stacked_broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data) throw (Exception);

    void stacked_broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data) throw (Exception);

    void flush_stacked_send_data(const Connection *c, flags_t flags) throw (Exception);
    void flush_stacked_broadcast_data(flags_t flags) throw (Exception);
    void flush_stacked_broadcast_data_synced(flags_t flags) throw (Exception);

    void send_data(const Connection *c, unsigned char tournament_id, command_t cmd, flags_t flags,
        data_len_t len, const void *data) throw (Exception);

    void broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags, data_len_t len,
        const void *data) throw (Exception);

    void broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags, data_len_t len,
        const void *data) throw (Exception);

    void set_server(Server *server);
    void reload_config(hostport_t port, pico_size_t num_players,
        const std::string& server_name, const std::string& password) throw (Exception);

    hostport_t get_port() const;

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
