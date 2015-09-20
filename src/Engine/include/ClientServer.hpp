#ifndef CLIENTSERVER_HPP
#define CLIENTSERVER_HPP

#include "MessageSequencer.hpp"
#include "TournamentCTF.hpp"
#include "TournamentDM.hpp"
#include "TournamentTDM.hpp"
#include "Player.hpp"
#include "GameProtocol.hpp"

#include <string>

class ClientServer : public MessageSequencer {
public:
    ClientServer(hostport_t port, pico_size_t num_players,
        const std::string& server_name, const std::string& password);
    ClientServer(hostaddr_t host, hostport_t port);
    virtual ~ClientServer();

protected:
    enum SyncState {
        PrepareMapsSync = 0,
        InMapsSync,
        MapsSynced
    };

    Tournament *tournament;
    GTransport *gtrans;
    GGameState *ggamestat;
    GPlayerState *gplayerstat;
    GPlayerInfo *gplayerinfo;
    GPlayerDescription *gplayerdesc;
    GPlaceObject *gplaceobject;

    Players players;

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

    hostport_t get_port() const;

private:
    char buffer[1024];
    GTransport *pb;
    int packet_len;
    hostport_t port;

    void stack_data(unsigned char tournament_id, command_t cmd, data_len_t len, const void *data);
};

#endif // CLIENTSERVER_HPP
