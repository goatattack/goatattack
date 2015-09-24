#ifndef SERVER_HPP
#define SERVER_HPP

#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Properties.hpp"
#include "Thread.hpp"
#include "ClientServer.hpp"
#include "TournamentFactory.hpp"
#include "ServerLogger.hpp"
#include "ServerAdmin.hpp"

#include <vector>
#include <fstream>

class ServerException : public Exception {
public:
    ServerException(const char *msg) : Exception(msg) { }
    ServerException(std::string msg) : Exception(msg) { }
};

class Server : public Properties, public ClientServer, public Thread {
public:
    Server(Resources& resources, Subsystem& subsystem,
        hostport_t port, pico_size_t num_players, const std::string& server_name,
        GamePlayType type, const std::string& map_name, int duration, int warmup)
        throw (Exception);

    Server(Resources& resources, Subsystem& subsystem,
        const std::string& server_config_file) throw (Exception);

    virtual ~Server();

    void start() throw (ServerException);
    void stop();

private:
    struct ClientPak {
        ClientPak(const Resources::LoadedPak *pak) : pak(pak), remaining(0), synced(0) { }

        const Resources::LoadedPak *pak;
        datasize_t remaining;
        bool synced;
    };

    typedef std::vector<ClientPak> ClientPaks;

    struct PlayerClientPak {
        PlayerClientPak(Player *p) : p(p), f(0), current_client_pak(0) { }

        Player *p;
        FILE *f;
        ClientPak *current_client_pak;
        ClientPaks client_paks;
    };

    typedef std::vector<PlayerClientPak> PlayerClientPaks;

    struct PlayerStats {
        unsigned char tournament_id;
        GPlayerServerState server_state;
        GPlayerClientServerState client_server_state;
        hostaddr_t host;
        std::string player_name;
        gametime_t disconnect_time;
    };

    typedef std::vector<MapConfiguration> MapConfigurations;
    typedef std::vector<PlayerStats *> HeldPlayerStats;

    /* in ctor */
    Resources& resources;
    Subsystem& subsystem;
    TournamentFactory factory;

    int nbr_logout_msg;
    bool running;
    const MapConfiguration *current_config;
    int score_board_counter;
    bool warmup;
    bool hold_disconnected_players;
    int reconnect_kills;
    int hdp_counter;
    hostaddr_t master_server;
    int ms_counter;
    UDPSocket master_socket;
    size_t rotation_current_index;
    std::string team_red_name;
    std::string team_blue_name;
    std::ofstream *log_file;
    ServerLogger logger;
    ServerAdmin *server_admin;

    MapConfigurations map_configs;
    HeldPlayerStats held_player_stats;
    PlayerClientPaks player_client_paks;

    char pak_buffer[PacketMaxSize];

    bool select_map();
    void cleanup_held_players();
    void quit_client(const Connection *c, Player *p, const std::string& message);

    void sync_client(const Connection *c, Player *p);

    PlayerClientPak *get_player_client_pak(Player *p);
    void process_sync_pak(const Connection *c, Player *p) throw (ServerException);
    ClientPak *get_unsynced_client_pak(PlayerClientPak *pcpak);
    void destroy_paks(Player *p);

    std::ostream& create_log_stream();

    void parse_command(const Connection *c, Player *p, data_len_t len, void *data) throw (ServerAdminException);


    /* implements Thread */
    virtual void thread();

    /* implements MessageSequencer */
    virtual void event_login(const Connection *c, data_len_t len, void *data) throw (Exception);
    virtual void event_data(const Connection *c, data_len_t len, void *data) throw (Exception);
    virtual void event_logout(const Connection *c, LogoutReason reason) throw (Exception);
};

class ScopeServer {
private:
    ScopeServer(const ScopeServer&);
    ScopeServer& operator=(const ScopeServer&);

public:
    ScopeServer(Server& server);
    virtual ~ScopeServer();

private:
    Server& server;
};

#endif // SERVER_HPP
