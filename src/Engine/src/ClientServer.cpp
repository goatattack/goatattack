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

#include "ClientServer.hpp"
#include "Server.hpp"

#include <cassert>

ClientServer::ClientServer(I18N& i18n, hostport_t port, pico_size_t num_players,
    const std::string& server_name, const std::string& password)
    : MessageSequencer(i18n, port, num_players, server_name, password),
      server(0), tournament(0),
      gtrans(reinterpret_cast<GTransport *>(buffer)),
      pb(gtrans), packet_len(0), port(port), has_temp_map_config(false) { }

ClientServer::ClientServer(I18N& i18n, hostaddr_t host, hostport_t port)
    : MessageSequencer(i18n, host, port), server(0), tournament(0),
      gtrans(reinterpret_cast<GTransport *>(buffer)),
      pb(gtrans), packet_len(0), port(port), has_temp_map_config(false) { }

ClientServer::~ClientServer() {
    if (tournament) {
        tournament->set_gui_is_destroyed(true);
        delete tournament;
        tournament = 0;
    }
}

Tournament *ClientServer::get_tournament() const {
    return tournament;
}

Players& ClientServer::get_players() {
    return players;
}

void ClientServer::delete_tournament() {
    if (tournament) {
        delete tournament;
        tournament = 0;
    }
}

bool ClientServer::use_temporary_map_config() const {
    return has_temp_map_config;
}

void ClientServer::set_temporary_map_config(bool state) {
    has_temp_map_config = state;
}

MapConfiguration& ClientServer::get_temporary_map_config() {
    return temp_map_config;
}

void ClientServer::stacked_send_data(const Connection *c, unsigned char tournament_id, command_t cmd,
    flags_t flags, data_len_t len, const void *data)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_send_data(c, flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::stacked_broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags,
    data_len_t len, const void *data)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_broadcast_data(flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::stacked_broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags,
    data_len_t len, const void *data)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_broadcast_data_synced(flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::flush_stacked_send_data(const Connection *c, flags_t flags) {
    if (packet_len) {
        push(c, flags, packet_len, gtrans);
        pb = gtrans;
        packet_len = 0;
    }
}

void ClientServer::flush_stacked_broadcast_data(flags_t flags) {
    if (packet_len) {
        broadcast(flags, packet_len, gtrans);
        pb = gtrans;
        packet_len = 0;
    }
}

void ClientServer::flush_stacked_broadcast_data_synced(flags_t flags) {
    if (packet_len) {
        for (Players::iterator it = players.begin(); it != players.end(); it++) {
            Player *p = *it;
            if (p->client_synced) {
                push(p->get_connection(), flags, packet_len, gtrans);
            }
        }
        pb = gtrans;
        packet_len = 0;
    }
}

void ClientServer::send_data(const Connection *c, unsigned char tournament_id,
    command_t cmd, flags_t flags, data_len_t len, const void *data)
{
    if (c) {
        gtrans->cmd = cmd;
        gtrans->tournament_id = tournament_id;
        gtrans->flags = 0;
        gtrans->len = len;
        if (data != gtrans->data) {
            memcpy(gtrans->data, data, len);
        }
        gtrans->to_net();
        push(c, flags, GTransportLen + len, gtrans);
    }
}

void ClientServer::broadcast_data(unsigned char tournament_id, command_t cmd,
    flags_t flags, data_len_t len, const void *data)
{
    gtrans->cmd = cmd;
    gtrans->tournament_id = tournament_id;
    gtrans->flags = 0;
    gtrans->len = len;
    if (data != gtrans->data) {
        memcpy(gtrans->data, data, len);
    }
    gtrans->to_net();
    broadcast(flags, GTransportLen + len, gtrans);
}

void ClientServer::broadcast_data_synced(unsigned char tournament_id, command_t cmd,
    flags_t flags, data_len_t len, const void *data)
{
    gtrans->cmd = cmd;
    gtrans->tournament_id = tournament_id;
    gtrans->flags = 0;
    gtrans->len = len;
    if (data != gtrans->data) {
        memcpy(gtrans->data, data, len);
    }
    gtrans->to_net();
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->client_synced) {
            push(p->get_connection(), flags, GTransportLen + len, gtrans);
        }
    }
}

hostport_t ClientServer::get_port() const {
    return port;
}

char *ClientServer::create_text(const std::string& txt, size_t& sz) {
    sz = txt.length() + 1;
    char *p = new char[sz];
    memcpy(p, txt.c_str(), sz);
    return p;
}

I18NText ClientServer::get_logout_text_id(LogoutReason reason) {
    I18NText id = I18N_CLIENT_LOGGED_OUT;
    switch (reason) {
        case LogoutReasonRegular:
            id = I18N_CLIENT_LOGGED_OUT;
            break;

        case LogoutReasonPingTimeout:
            id = I18N_CLIENT_PING_TIMEOUT;
            break;

        case LogoutReasonTooManyResends:
            id = I18N_CLIENT_TOO_MANY_RESENDS;
            break;

        case LogoutReasonApplicationQuit:
            id = I18N_CLIENT_APPLICATION_LAYER;
            break;
    }

    return id;
}

I18NText ClientServer::get_logout_text_id_client(LogoutReason reason) {
    I18NText id = I18N_CLIENT_LOGGED_OUT_RPL;
    switch (reason) {
        case LogoutReasonRegular:
            id = I18N_CLIENT_LOGGED_OUT_RPL;
            break;

        case LogoutReasonPingTimeout:
            id = I18N_CLIENT_PING_TIMEOUT_RPL;
            break;

        case LogoutReasonTooManyResends:
            id = I18N_CLIENT_TOO_MANY_RESENDS_RPL;
            break;

        case LogoutReasonApplicationQuit:
            id = I18N_CLIENT_APPLICATION_LAYER_RPL;
            break;
    }

    return id;
}

void ClientServer::set_server(Server *server) {
    this->server = server;
}

void ClientServer::reload_config(hostport_t port, pico_size_t num_players,
    const std::string& server_name, const std::string& password)
{
    if (!server) {
        throw Exception("No server, reloading settings failed.");
    }
    MessageSequencer::new_settings(port, num_players, server_name, password);
    this->port = port;
    server->reload_config();
}

void ClientServer::stack_data(unsigned char tournament_id, command_t cmd, data_len_t len, const void *data) {
    if (packet_len) {
        pb->flags |= TransportFlagMorePackets;
        unsigned char *tpb = reinterpret_cast<unsigned char *>(pb);
        tpb += GTransportLen;
        tpb += ntohs(pb->len);
        pb = reinterpret_cast<GTransport *>(tpb);
    }

    pb->cmd = cmd;
    pb->tournament_id = tournament_id;
    pb->flags = 0;
    pb->len = len;
    assert(data != pb->data && "Source and destination buffer must not be the same in stacked sending");
    memcpy(pb->data, data, len);
    pb->to_net();

    packet_len += GTransportLen + len;
}
