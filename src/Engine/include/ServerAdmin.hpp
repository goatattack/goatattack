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

#ifndef _SERVERADMIN_HPP_
#define _SERVERADMIN_HPP_

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
    std::string admin_password;

    static ServerCommand server_commands[];

    void update_configuration(const Connection *c) throw (Exception);

    void sc_op(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_deop(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_kick(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_ban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_unban(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_next(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_map(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_reload(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_save(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_get(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_set(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_reset(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);
    void sc_vote(const Connection *c, Player *p, const std::string& params) throw (ServerAdminException);

    void check_if_authorized(Player *p) throw (ServerAdminException);
    void check_if_params(const std::string& params) throw (ServerAdminException);
    void check_if_no_params(const std::string& params) throw (ServerAdminException);
    void throw_illegal_parameters() throw (ServerAdminException);
    void throw_not_implemented() throw (ServerAdminException);
};

#endif
