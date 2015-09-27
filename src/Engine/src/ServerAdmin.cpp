#include "ServerAdmin.hpp"
#include "Utils.hpp"

#include <cstdlib>

ServerAdmin::ServerCommand ServerAdmin::server_commands[] = {
    { "op", &ServerAdmin::sc_op },
    { "deop", &ServerAdmin::sc_deop },
    { "kick", &ServerAdmin::sc_kick },
    { "ban", &ServerAdmin::sc_ban },
    { "unban", &ServerAdmin::sc_unban },
    { "next", &ServerAdmin::sc_next },
    { "map", &ServerAdmin::sc_map },
    { "reload", &ServerAdmin::sc_reload },
    { 0, 0 }
};

ServerAdmin::ServerAdmin(Resources& resources, ClientServer& client_server,
    Properties& properties)
    : resources(resources), server(client_server), properties(properties),
      admin_password(properties.get_value("admin_password")) { }

ServerAdmin::ServerAdmin(Resources& resources, ClientServer& client_server,
    Properties& properties, const std::string& admin_password)
    : resources(resources), server(client_server), properties(properties),
      admin_password(admin_password) { }

ServerAdmin::~ServerAdmin() { }

void ServerAdmin::execute(const Connection *c, Player *p, std::string cmd, std::string params)
    throw (ServerAdminException)
{
    instant_trim(cmd);
    instant_trim(params);
    cmd = lowercase(cmd);
    bool found = false;
    ServerCommand *sc = server_commands;
    while (sc->command) {
        if (!strcmp(sc->command, cmd.c_str())) {
            found = true;
            (this->*sc->function)(c, p, params);
            break;
        }
        sc++;
    }

    if (!found) {
        throw ServerAdminException("Unknown server command: " + cmd);
    }
}

/* server functions */
void ServerAdmin::sc_op(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    if (params != admin_password) {
        throw ServerAdminException("Wrong admin password");
    }
    if (p->server_admin) {
        throw ServerAdminException("You're already admin");
    }
    p->server_admin = true;
    std::string msg(p->get_player_name() + " is now server admin");
    server.broadcast_data(0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
}

void ServerAdmin::sc_deop(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    check_if_authorized(p);
    p->server_admin = false;
    std::string msg(p->get_player_name() + " left the server admin mode");
    server.broadcast_data(0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
}

void ServerAdmin::sc_kick(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    check_if_authorized(p);
    check_if_params(params);
    Players& players = server.get_players();
    bool found = false;
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *v = *it;
        std::string pn(v->get_player_name());
        instant_trim(pn);
        if (pn == params) {
            if (v->get_connection() == c) {
                throw ServerAdminException("You cannot kick yourself");
            }
            found = true;
            server.kill(v->get_connection());
            std::string msg(p->get_player_name() + " kicked " + pn);
            server.broadcast_data(0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
            break;
        }
    }
    if (!found) {
        std::string msg("Player not found: " + params);
        server.send_data(c, 0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
    }
}

void ServerAdmin::sc_ban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    throw_not_implemented();
}

void ServerAdmin::sc_unban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    throw_not_implemented();
}

void ServerAdmin::sc_next(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    check_if_authorized(p);
    check_if_no_params(params);
    server.delete_tournament();
    std::string msg(p->get_player_name() + " started next map");
    server.broadcast_data(0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
}

void ServerAdmin::sc_map(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    check_if_authorized(p);
    StringTokens tokens = tokenize(params, ' ');
    if (tokens.size() != 3) {
        throw ServerAdminException("Usage: /map <map_name> <warmup> <duration>");
    }

    Map *map = 0;
    try {
        map = resources.get_map(tokens[0]);
    } catch (const ResourcesException& e) {
        throw ServerAdminException(e.what());
    }

    int warmup = atoi(tokens[1].c_str());
    int duration = atoi(tokens[2].c_str());

    if (warmup < 0 || warmup > 120 || duration < 1 || duration > 120) {
        throw_illegal_parameters();
    }

    MapConfiguration& config = server.get_temporary_map_config();
    config.map_name = map->get_name();
    config.type = map->get_game_play_type();
    config.warmup_in_seconds = warmup;
    config.duration = duration;
    server.set_temporary_map_config(true);

    server.delete_tournament();
    std::string msg(p->get_player_name() + " started map " + config.map_name);
    server.broadcast_data(0, GPCTextMessage, NetFlagsReliable, msg.length(), msg.c_str());
}

void ServerAdmin::sc_reload(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException) {
    throw_not_implemented();
}

/* helper functions */
void ServerAdmin::check_if_authorized(Player *p) throw (ServerAdminException) {
    if (!p->server_admin) {
        throw ServerAdminException("You're not authorized to do that");
    }
}

void ServerAdmin::check_if_params(const std::string& params) throw (ServerAdminException) {
    if (!params.length()) {
        throw ServerAdminException("Parameter missing");
    }
}

void ServerAdmin::check_if_no_params(const std::string& params) throw (ServerAdminException) {
    if (params.length()) {
        throw ServerAdminException("No parameters needed");
    }
}

void ServerAdmin::throw_illegal_parameters() throw (ServerAdminException) {
    throw ServerAdminException("Illegal parameters");
}
void ServerAdmin::throw_not_implemented() throw (ServerAdminException) {
    throw ServerAdminException("Not implemented yet");
}