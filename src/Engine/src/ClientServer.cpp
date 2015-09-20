#include "ClientServer.hpp"

#include <cassert>

ClientServer::ClientServer(hostport_t port, pico_size_t num_players,
    const std::string& server_name, const std::string& password)
    : MessageSequencer(port, num_players, server_name, password), tournament(0),
      gtrans(reinterpret_cast<GTransport *>(buffer)),
      ggamestat(reinterpret_cast<GGameState *>(gtrans->data)),
      gplayerstat(reinterpret_cast<GPlayerState *>(gtrans->data)),
      gplayerinfo(reinterpret_cast<GPlayerInfo *>(gtrans->data)),
      gplayerdesc(reinterpret_cast<GPlayerDescription *>(gtrans->data)),
      gplaceobject(reinterpret_cast<GPlaceObject *>(gtrans->data)),
      pb(gtrans), packet_len(0), port(port) { }

ClientServer::ClientServer(hostaddr_t host, hostport_t port)
    : MessageSequencer(host, port), tournament(0),
      gtrans(reinterpret_cast<GTransport *>(buffer)),
      ggamestat(reinterpret_cast<GGameState *>(gtrans->data)),
      gplayerstat(reinterpret_cast<GPlayerState *>(gtrans->data)),
      gplayerinfo(reinterpret_cast<GPlayerInfo *>(gtrans->data)),
      gplayerdesc(reinterpret_cast<GPlayerDescription *>(gtrans->data)),
      gplaceobject(reinterpret_cast<GPlaceObject *>(gtrans->data)),
      pb(gtrans), packet_len(0), port(port) { }

ClientServer::~ClientServer() {
    if (tournament) {
        tournament->set_gui_is_destroyed(true);
        delete tournament;
    }
}

void ClientServer::stacked_send_data(const Connection *c, unsigned char tournament_id, command_t cmd,
    flags_t flags, data_len_t len, const void *data) throw (Exception)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_send_data(c, flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::stacked_broadcast_data(unsigned char tournament_id, command_t cmd, flags_t flags,
    data_len_t len, const void *data) throw (Exception)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_broadcast_data(flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::stacked_broadcast_data_synced(unsigned char tournament_id, command_t cmd, flags_t flags,
    data_len_t len, const void *data) throw (Exception)
{
    int sz = packet_len + GTransportLen + len;
    if (sz > PacketMaxSize) {
        flush_stacked_broadcast_data_synced(flags);
    }
    stack_data(tournament_id, cmd, len, data);
}

void ClientServer::flush_stacked_send_data(const Connection *c, flags_t flags) throw (Exception) {
    if (packet_len) {
        push(c, flags, packet_len, gtrans);
        pb = gtrans;
        packet_len = 0;
    }
}

void ClientServer::flush_stacked_broadcast_data(flags_t flags) throw (Exception) {
    if (packet_len) {
        broadcast(flags, packet_len, gtrans);
        pb = gtrans;
        packet_len = 0;
    }
}

void ClientServer::flush_stacked_broadcast_data_synced(flags_t flags) throw (Exception) {
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
    command_t cmd, flags_t flags, data_len_t len, const void *data) throw (Exception)
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
    flags_t flags, data_len_t len, const void *data) throw (Exception)
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
    flags_t flags, data_len_t len, const void *data) throw (Exception)
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
