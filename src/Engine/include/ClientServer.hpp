#ifndef CLIENTSERVER_HPP
#define CLIENTSERVER_HPP

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
    GGameState *ggamestat;
    GPlayerState *gplayerstat;
    GPlayerInfo *gplayerinfo;
    GPlayerDescription *gplayerdesc;
    GPlaceObject *gplaceobject;

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
