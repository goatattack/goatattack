#ifndef SERVERADMIN_HPP
#define SERVERADMIN_HPP

#include "Exception.hpp"
#include "Resources.hpp"
#include "ClientServer.hpp"
#include "Properties.hpp"

class ServerAdminException : public Exception {
public:
    ServerAdminException(const char *msg) : Exception(msg) { }
    ServerAdminException(std::string msg) : Exception(msg) { }
};

class ServerAdmin {
private:
    ServerAdmin(const ServerAdmin&);
    ServerAdmin& operator=(const ServerAdmin&);

public:
    ServerAdmin(Resources& resources, ClientServer& client_server,
        Properties& properties);

    ServerAdmin(Resources& resources, ClientServer& client_server,
        Properties& properties, const std::string& admin_password);

    virtual ~ServerAdmin();

    void execute(const Connection *c, Player *p, std::string cmd, std::string params) throw (ServerAdminException);

private:
    typedef void (ServerAdmin::*AdminFunction)(const Connection *c, Player *p, const std::string& params);

    struct ServerCommand {
        const char *command;
        AdminFunction function;
    };

    Resources& resources;
    ClientServer& server;
    Properties& properties;
    const std::string& admin_password;

    static ServerCommand server_commands[];

    void sc_op(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_deop(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_kick(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_ban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_unban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_next(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_map(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_reload(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_get(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_set(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_reset(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);

    void check_if_authorized(Player *p) throw (ServerAdminException);
    void check_if_params(const std::string& params) throw (ServerAdminException);
    void check_if_no_params(const std::string& params) throw (ServerAdminException);
    void throw_illegal_parameters() throw (ServerAdminException);
    void throw_not_implemented() throw (ServerAdminException);
};

#endif